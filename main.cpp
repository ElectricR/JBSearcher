#include <wx/wx.h>
#include <iostream>
#include <condition_variable>
#include <fstream>
#include <mutex>
#include <atomic>
#include <thread>
#include <string>
#include <vector>

class SearchHandler;
class SearcherFrame;


void SearcherRoutine(SearchHandler &handler);

class SearchHandler {
public:
    SearchHandler(SearcherFrame *frame):
        frame_(frame),
        th_(std::thread(SearcherRoutine, std::ref(*this)))
    {}

    ~SearchHandler() {
        m_.lock();
        quit_called_ = true;
        cv_.notify_one();
        m_.unlock();
        th_.join();
    }

    void set_input(std::string&& str) {
        std::unique_lock ul(m_);
        input_changed_ = true;
        input_string_ = std::move(str);
        out_string_.clear();
        cv_.notify_one();
    }

    std::string unsafe_get_input() {
        input_changed_ = false;
        return input_string_;
    }

    void flush_output(std::string& str) {
        std::unique_lock ul(m_);
        str.swap(out_string_);
    }
    
    void unsafe_set_output(std::string&& str);

    std::mutex m_;
    std::condition_variable cv_;
    std::atomic<bool> input_changed_;
    std::atomic<bool> quit_called_ = false;
private:
    SearcherFrame *frame_;
    std::string input_string_;
    std::string out_string_;
    std::thread th_;
};

bool is_substr(const std::string& pattern, const std::string& str) {
    size_t i = 0;
    size_t j = 0;
    while (i != pattern.size() && j != str.size()) {
        if (pattern[i] == str[j]) {
            ++i;
        }
        ++j;
    }
    return i == pattern.size();
}


void SearcherRoutine(SearchHandler &handler) {
    while (!handler.quit_called_) {
        std::ifstream ifs("dict.txt");
        std::vector<std::string> words_;
        std::string s;
        while (std::getline(ifs, s)) {
            words_.push_back(std::move(s));
        }
        ifs.close();
        
        std::unique_lock ul(handler.m_);
        handler.cv_.wait(ul, [&] { return handler.quit_called_ || handler.input_changed_; });
        if (handler.input_changed_) {
            std::string pattern = handler.unsafe_get_input();
            ul.unlock();
            if (pattern.size() > 1) {
                std::string out_string;
                for (std::string &str : words_) {
                    if (is_substr(pattern, str)) {
                        if (out_string.size() != 0) {
                            out_string += '\n';
                        }
                        out_string += str;
                    }
                }
                ul.lock();
                if (!handler.input_changed_) {
                    handler.unsafe_set_output(std::move(out_string));
                }
                ul.unlock();
            }
        }
    }
}

class SearcherFrame:
    public wxFrame
{
public:
    SearcherFrame():
        wxFrame(NULL, wxID_ANY, "Searcher", wxDefaultPosition, wxSize(600, 600))
    {
        parent_ = new wxPanel(this, wxID_ANY);
        input_ = new wxTextCtrl(parent_, 1, wxT(""), wxPoint(100, 100));
        result_ = new wxTextCtrl(parent_, -1, wxT(""), wxPoint(100, 150), wxSize(200, 400), wxTE_MULTILINE);

        Connect(1, wxEVT_TEXT, wxCommandEventHandler(SearcherFrame::OnTextChange));
        Connect(wxEVT_THREAD, wxCommandEventHandler(SearcherFrame::OnThreadEvent));
        
        Centre();

        handler_ = std::make_unique<SearchHandler>(this);
    }

    void OnTextChange(wxCommandEvent &) {
        std::string str = input_->GetValue().ToStdString();
        if (str.size() > 1) {
            result_->SetValue("...");
        }
        else {
            result_->SetValue("");
        }
        handler_->set_input(std::move(str));
    }

    void OnThreadEvent(wxCommandEvent &) {
        std::string str;
        handler_->flush_output(str);
        result_->SetValue(str);
    }

private:
    wxPanel *parent_;
    wxTextCtrl *result_;
    wxTextCtrl *input_;
    std::unique_ptr<SearchHandler> handler_;
};


void SearchHandler::unsafe_set_output(std::string&& str) {
    out_string_ = str;
    wxQueueEvent(frame_, new wxThreadEvent);
}

class SearcherApp:
    public wxApp
{
public:
    bool OnInit() override {
        SearcherFrame *frame = new SearcherFrame();
        frame->Show(true);
        return true;
    }
};


wxIMPLEMENT_APP(SearcherApp);
