#pragma once

#include <fstream>
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <windows.h>
#include <iomanip>

using std::string;
using std::ofstream;
using std::ifstream;
using std::vector;
using std::list;

class archiver {
    struct node {
        node* left = NULL;
        node* right = NULL;
        unsigned char c;
        long long amount = 0;

        node() : left(nullptr), right(nullptr) {}
        node(char c, long long cnt) :c(c), amount(cnt) {}
        node(node* left_node, node* right_node) {
            left = left_node;
            right = right_node;
            amount = left_node -> amount + right_node -> amount;
        }
    };

    struct file_node {
        bool is_file_;
        long long size_;
        string name_;
        vector <file_node*> files_;
    };

    struct my_compare {
        bool operator()(const node* l, const node* r) const { return l -> amount < r -> amount; }
    };

    void add_path(string path, ofstream &output);

    long long add_file(string name);

    string get_file(string path);

    string get_path(string path);

    void make_file_tree(file_node*& current_node, string path, string name, ofstream &output);

    void make_tree();

    void build_table(node* current_node, string &code);

    void view_char(node* current_node, ofstream &output);

    void encode_tree(node* current_node, ofstream &output, int &buf, int &bit, long long &cnt);

    void files_compress(file_node*& current_node, string path, ofstream& output, int &buf, int &bit);

    void compress(string file, ofstream &output, int &buf, int &bit);

    void delete_tree(node*& current_node);

    void delete_file_tree(file_node*& current_node);

    void read_tree(node*& current_node, string &code, int &c_index, string &chars, ifstream &input, int &buf, int &bit_index);

    bool read_file_tree(vector<file_node*>* root_dir, ifstream &input);

    void tree_decompress(file_node* current_node, string path, ifstream &input, int &buf, int &bit_index);

    void decompress(string file_path, long long char_cnt, ifstream &input, int &buf, int &bit_index);

    long long frequencies_[256] = {};
    node* root_ = NULL;
    string root_path_;
    string table_[256];
    file_node* root_file_;

public:
    archiver();

    void pack(string path_from, string path_to);

    void un_pack(string path, string dir);

    void free();

    ~archiver();
};

