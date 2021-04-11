#include <wx/wx.h>
#include <iostream>
#include <condition_variable>
#include <fstream>
#include <mutex>
#include <atomic>
#include <thread>
#include <string>
#include <vector>

#include "algorithms.h"

class SearchHandler;
class SearcherFrame;

void SearcherRoutine(SearchHandler &handler);

//Class for data exchange and thread handling
class SearchHandler {
public:
    // Creates thread upon creation
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

    // Assign new input from input test field
    void set_input(std::string&& str) {
        std::unique_lock ul(m_);
        input_changed_ = true;
        input_string_ = std::move(str);
        cons_out_string_.clear();
        noncons_out_string_.clear();
        cv_.notify_one();
    }

    std::string unsafe_get_input() {
        input_changed_ = false;
        return input_string_;
    }

    // Combine consecutive and nonconsecutive results
    // in order to write it to GUI buffer
    void flush_output(std::string& str) {
        std::unique_lock ul(m_);
        str.swap(cons_out_string_);
        if (str.size()) {
            str += "----------------\n";
        }
        str += std::move(noncons_out_string_);
    }
    
    void unsafe_set_output(std::string&&, std::string&&);

    std::mutex m_;
    std::condition_variable cv_;
    std::atomic<bool> input_changed_;
    std::atomic<bool> quit_called_ = false;
private:
    SearcherFrame *frame_;
    std::string input_string_;
    std::string cons_out_string_; // Words with consecutive pattern matching
    std::string noncons_out_string_; //Words with nonconsecutive pattern matching
    std::thread th_; // Searcher thread
};


// Main function for searcher thread
// 
// Reads dictionary from disk
// Then pattern from SearchHandler and calculates if 
// pattern is consecutive or nonconsecutive substring of each word
//
void SearcherRoutine(SearchHandler &handler) {
    while (!handler.quit_called_) {
        // Handle disk input
        std::ifstream ifs("dict.txt");
        std::vector<std::string> words;
        std::string s;
        std::string con_str;
        while (std::getline(ifs, s)) {
            con_str += s + '\n';
            words.push_back(std::move(s));
        }
        ifs.close();
        
        // Sleep if there is no work to do
        std::unique_lock ul(handler.m_);
        handler.cv_.wait(ul, [&] { return handler.quit_called_ || handler.input_changed_; });
        
        if (handler.input_changed_) {
            std::string pattern = handler.unsafe_get_input();
            ul.unlock();

            // Check only >2 symbol patterns
            if (pattern.size() > 1) {
                std::string cons_out_string;
                std::string noncons_out_string;

                // Apply two algorithms
                std::vector<bool> cons = algo::KnuthMorrisPratt(pattern, con_str);
                std::vector<bool> noncons = algo::nonconsecutive_substring_algorithm(pattern, con_str);
                // Write consecutive and nonconsecutive substrings to separated buffers
                for (size_t i = 0; i != words.size(); ++i) {
                    if (cons[i]) {
                        cons_out_string += std::move(words[i] + '\n');
                    }
                    else if (noncons[i]) {
                        noncons_out_string += std::move(words[i] + '\n');
                    }
                }

                // Dump calculations
                ul.lock();
                if (!handler.input_changed_) {
                    handler.unsafe_set_output(std::move(cons_out_string), std::move(noncons_out_string));
                }
                ul.unlock();
            }
        }
    }
}

// UI frame for managing input and output
class SearcherFrame:
    public wxFrame
{
public:
    SearcherFrame():
        wxFrame(NULL, wxID_ANY, "Searcher", wxDefaultPosition, wxSize(600, 600))
    {
        parent_ = new wxPanel(this, wxID_ANY);
        input_ = new wxTextCtrl(parent_, 1, wxT(""), wxPoint(100, 100), wxSize(200, wxDefaultCoord));
        result_ = new wxTextCtrl(parent_, -1, wxT(""), wxPoint(100, 150), wxSize(400, 400), wxTE_MULTILINE);

        Connect(1, wxEVT_TEXT, wxCommandEventHandler(SearcherFrame::OnTextChange));
        Connect(wxEVT_THREAD, wxCommandEventHandler(SearcherFrame::OnThreadEvent));
        
        Centre();

        handler_ = std::make_unique<SearchHandler>(this);
    }

    // Input bar text change handling
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

    // Finish calculation event handling
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


void SearchHandler::unsafe_set_output(std::string&& cons, std::string&& noncons) {
    cons_out_string_ = std::move(cons);
    noncons_out_string_ = std::move(noncons);
    wxQueueEvent(frame_, new wxThreadEvent);
}

// WX main app
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
