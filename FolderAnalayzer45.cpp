// Samket Ateklit OJ3892| Yafet Getachew PZ2443
//DRBSE2401
#include <iostream>
#include <filesystem>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <fstream>

namespace fs = std::filesystem;
using namespace std;


struct FileInfo {
    string path;       
    string extension;  
    uintmax_t size;    
    fs::file_time_type mod_time; 
};

vector<FileInfo> allFiles; 


string formatSize(uintmax_t size) {
    double s = size;
    if (s < 1024) return to_string(s) + " B";
    s /= 1024;
    if (s < 1024) return to_string(s) + " KB";
    s /= 1024;
    if (s < 1024) return to_string(s) + " MB";
    s /= 1024;
    return to_string(s) + " GB";
}

 
void scanFolder(const fs::path& path) {
    try {
        for (auto& entry : fs::directory_iterator(path)) {
            if (entry.is_directory()) {
                scanFolder(entry.path()); 
            } else if (entry.is_regular_file()) {
                
                FileInfo f;
                f.path = entry.path().string();
                f.extension = entry.path().extension().string();
                f.size = entry.file_size();
                f.mod_time = entry.last_write_time();
                allFiles.push_back(f);
            }
        }
    } catch (fs::filesystem_error &e) {
        
        cerr << "Can't access " << path << ": " << e.what() << endl;
    }
}


void printSummary() {
    cout << "\n===== FILE SYSTEM SUMMARY =====\n";
    cout << "Total files: " << allFiles.size() << "\n";

    uintmax_t totalSize = 0;
    map<string, int> extCount;       
    map<string, uintmax_t> extSize;  

    for (auto &f : allFiles) {
        totalSize += f.size;
        extCount[f.extension]++;
        extSize[f.extension] += f.size;
    }

    cout << "Total size: " << formatSize(totalSize) << "\n";

    
    vector<pair<string,int>> sortedExt(extCount.begin(), extCount.end());
    sort(sortedExt.begin(), sortedExt.end(), [](auto &a, auto &b){ return a.second > b.second; });

    cout << "\nTop file types:\n";
    int limit = min(5, (int)sortedExt.size());
    for(int i=0;i<limit;i++)
        cout << sortedExt[i].first << " : " << sortedExt[i].second 
             << " files, " << formatSize(extSize[sortedExt[i].first]) << "\n";

    
    sort(allFiles.begin(), allFiles.end(), [](FileInfo &a, FileInfo &b){ return a.size > b.size; });
    cout << "\nTop 10 largest files:\n";
    for(int i=0; i<min(10,(int)allFiles.size()); i++)
        cout << allFiles[i].path << " : " << formatSize(allFiles[i].size) << "\n";
}


void writeJSON(const string &filename) {
    ofstream out(filename);
    out << "{\n";
    out << "  \"total_files\": " << allFiles.size() << ",\n";
    
    uintmax_t totalSize = 0;
    map<string, int> extCount;
    map<string, uintmax_t> extSize;

    for (auto &f : allFiles) {
        totalSize += f.size;
        extCount[f.extension]++;
        extSize[f.extension] += f.size;
    }

    out << "  \"total_size\": " << totalSize << ",\n";

    out << "  \"file_types\": {\n";
    int cnt = 0;
    for (auto &p : extCount) {
        out << "    \"" << p.first << "\": { \"count\": " << p.second 
            << ", \"size\": " << extSize[p.first] << "}";
        cnt++;
        if(cnt<extCount.size()) out << ",";
        out << "\n";
    }
    out << "  },\n";

    out << "  \"largest_files\": [\n";
    int limit = min(10, (int)allFiles.size());
    for(int i=0;i<limit;i++){
        out << "    {\"path\": \"" << allFiles[i].path << "\", \"size\": " << allFiles[i].size << "}";
        if(i<limit-1) out << ",";
        out << "\n";
    }
    out << "  ]\n";
    out << "}\n";
    out.close();
    cout << "\nJSON report written to " << filename << "\n";
}

int main() {
    string folder;
    cout << "Enter folder path to scan: ";
    getline(cin, folder);

    if(!fs::exists(folder)) {
        cerr << "Folder does not exist!\n";
        return 1;
    }

    scanFolder(folder);   
    printSummary();       

    string jsonFile = "report.json";
    writeJSON(jsonFile);  

    return 0;
}
