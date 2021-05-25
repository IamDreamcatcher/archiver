#include "archiver.h"
#include <QDebug>

string archiver::get_file(string path) {
    size_t found = path.find_last_of("/\\");
	return path.substr(found + 1);
}

string archiver::get_path(string path) {
	size_t found = path.find_last_of("/\\");
	return path.substr(0, found + 1);
}

void archiver::make_file_tree(file_node*& current_node, string path, string name, ofstream& output) {
    current_node = new file_node();

	string concat = path + name;
    auto isDirectory = GetFileAttributesA((path + name).c_str());
    if (isDirectory == INVALID_FILE_ATTRIBUTES) {
		output.close();
        throw (string)"Error reading file for archiving";
	}
    if (isDirectory == FILE_ATTRIBUTE_DIRECTORY) {
        current_node -> is_file_ = false;
        current_node -> name_ = name;

        output << '/' << current_node -> name_ << '<' << "/ ";

        WIN32_FIND_DATAA find_file_data;
        HANDLE h_find;

		string concat = path + name + "\\*.*";
        h_find = FindFirstFileA((path + name + "\\*.*").c_str(), &find_file_data);
        if (h_find != INVALID_HANDLE_VALUE) {
			do {
                if (find_file_data.cFileName[0] == '.')
					continue;
                current_node -> files_.push_back(nullptr);
                make_file_tree(current_node -> files_.back(), path + name + "\\", find_file_data.cFileName, output);
            } while (FindNextFileA(h_find, &find_file_data));

            FindClose(h_find);
		}

		output << "/>/ ";
    } else {
        current_node -> is_file_ = true;
        current_node -> size_ = add_file(path + name);

        output << '/' << name << '/' << ' ' << current_node -> size_ << ' ';

        current_node -> name_ = name;
	}
}

void archiver::make_tree() {
    list<node*> nodes;
    for (int i = 0; i < 256; i++) {
        if (frequencies_[i] != 0) {
            nodes.push_back(new node((char)i, frequencies_[i]));
        }
    }
    if (nodes.size() == 1) {
        root_ = new node(0, nodes.front() -> amount);
        root_ -> left = nodes.front();
		return;
	}

    while (nodes.size() > 1) {
        nodes.sort(my_compare());

        node* first = nodes.front();
        nodes.pop_front();
        node* second = nodes.front();
        nodes.pop_front();

        node* vertices_union = new node(first, second);
        nodes.push_front(vertices_union);
	}

    root_ = nullptr;
    if (!nodes.empty()) {
        root_ = nodes.front();
	}

}

void archiver::build_table(node* current_node, string& code) {
    if (current_node -> left != nullptr) {
		code.push_back('0');
        build_table(current_node -> left, code);
		code.pop_back();
	}

    if (current_node -> right != nullptr) {
		code.push_back('1');
        build_table(current_node -> right, code);
		code.pop_back();
	}

    if (current_node -> left == nullptr && current_node -> right == nullptr) {
        table_[current_node -> c] = code;
    }
}

void archiver::view_char(node* current_node, ofstream& output) {
    if (current_node -> left != nullptr) {
        view_char(current_node -> left, output);
    }
    if (current_node -> right != nullptr) {
        view_char(current_node -> right, output);
    }
    if (current_node -> left == nullptr && current_node -> right == nullptr) {
        output << current_node -> c;
    }
}

void archiver::encode_tree(node* current_node, ofstream& output, int& buffer_mask, int& bit_index, long long& cnt) {
    if (cnt == 0) {
        return;
    }
    if (current_node -> left == nullptr && current_node -> right == nullptr) {
        buffer_mask = buffer_mask | (1 << (7 - bit_index));
        bit_index++;
        if (bit_index == 8) {
            output << (char)buffer_mask;
            bit_index = 0;
            buffer_mask = 0;
        }
        cnt--;
    } else {
        bit_index++;
        if (bit_index == 8) {
            output << (char)buffer_mask;
            buffer_mask = 0;
            bit_index = 0;
        }

        encode_tree(current_node -> left, output, buffer_mask, bit_index, cnt);
        encode_tree(current_node -> right, output, buffer_mask, bit_index, cnt);
    }
}

void archiver::files_compress(file_node*& current_node, string path, ofstream& output, int& buffer_mask, int& bit_index) {
    if (current_node -> is_file_) {
        compress(path + current_node -> name_, output, buffer_mask, bit_index);
    } else {
        for (int i = 0; i < (int) current_node -> files_.size(); i++) {
            files_compress(current_node -> files_[i], path + current_node -> name_ + '\\', output, buffer_mask, bit_index);
        }
	}
}

void archiver::compress(string file, ofstream& output, int& buffer_mask, int& bit_index) {
    if (output) {
		ifstream input(file, std::ios::binary);
        while (!input.eof()) {
			int c = input.get();
            if (c == -1) {
				break;
            }
            for (int j = 0; j < (int)table_[c].size(); j++) {
                buffer_mask = buffer_mask | ((table_[c][j] - '0') << (7 - bit_index));
                bit_index++;
                if (bit_index == 8) {
                    bit_index = 0;
                    output << (char)buffer_mask;
                    buffer_mask = 0;
				}
			}
		}
		input.close();
	}
}

void archiver::delete_tree(node*& current_node) {
    if (current_node == nullptr) {
		return;
    }
    delete_tree(current_node -> left);
    delete_tree(current_node -> right);
    delete current_node;
}

void archiver::delete_file_tree(file_node*& current_node) {
    if (current_node == nullptr) {
		return;
    }
    for (int i = 0; i < (int)current_node -> files_.size(); i++) {
        delete_file_tree(current_node -> files_[i]);
    }
    delete current_node;
}

void archiver::read_tree(node*& current_node, string& code, int& c_index, string& chars, ifstream& input, int& buffer_mask, int& bit_index) {
    current_node = new node();
    if (bit_index == 8) {
        buffer_mask = input.get();
        if (buffer_mask == -1) {
			input.close();
            throw (string)"Unpacking error";
		}
        bit_index = 0;
	}
    int bit = (buffer_mask >> (7 - bit_index)) & 1;
    bit_index++;
    if (bit == 0) {
		code.push_back('0');
        read_tree(current_node -> left, code, c_index, chars, input, buffer_mask, bit_index);
		code.pop_back();
        if (c_index == (int)chars.size())
			return;

		code.push_back('1');
        read_tree(current_node -> right, code, c_index, chars, input, buffer_mask, bit_index);
		code.pop_back();
    } else {
        current_node -> c = chars[c_index++];
	}
}

bool read_string_name(ifstream& input, string& name) {
	int size = 256;
    char first_char;
	name = "";
    if (input >> first_char) {
        if (first_char != '/') {
			return false;
        }
        int current_char = input.get();
        if (current_char == '/')
			return false;
        while (size >= 0 && current_char != '/' && current_char != -1) {
            name += (char)current_char;
			size--;
            current_char = input.get();
		}
        if (current_char == -1 || size < 0) {
			return false;
        }
		return true;
	}
	return false;
}

bool archiver::read_file_tree(vector <file_node*>* root_dir, ifstream& input)
{
	string name;
	long long size;
    while (read_string_name(input, name)) {
		if (name == ">")
			return true;

        if (name.back() == '<') {
            file_node* current_node = new file_node;
            current_node -> is_file_ = false;
            current_node -> name_ = name.substr(0, name.size() - 1);
            if (root_dir != NULL) {
                root_dir -> push_back(current_node);
            } else {
                root_file_ = current_node;
            }
            if (!read_file_tree(&(current_node -> files_), input)) {
				return false;
            }
            if (root_dir == nullptr) {
				return true;
            }
        } else {
			if (!(input >> size))
				return false;

            file_node* current_node = new file_node;

            current_node -> is_file_ = true;
            current_node -> name_ = name;
            current_node -> size_ = size;

            if (root_dir != nullptr) {
                root_dir -> push_back(current_node);
            } else {
                root_file_ = current_node;
				return true;
			}
		}
	}
	return false;
}

void archiver::tree_decompress(file_node* current_node, string path, ifstream& input, int& buffer_mask, int& bit_index) {
    if (current_node == nullptr) {
        throw (string)"Unpacking error";
	}
    if (current_node -> is_file_) {
        decompress(path + current_node -> name_, current_node -> size_, input, buffer_mask, bit_index);
    } else {
        CreateDirectoryA((path + current_node -> name_).c_str(), 0);
        for (int i = 0; i < (int)current_node -> files_.size(); i++) {
            tree_decompress(current_node -> files_[i], path + current_node -> name_ + '\\', input, buffer_mask, bit_index);
		}
	}
}

void archiver::decompress(string file_path, long long char_cnt, ifstream& input, int& buffer_mask, int& bit_index) {
    ofstream output(file_path, std::ios::binary);

    int processed_chars = 0;
    node* current_node = root_;

    if (!output) {
        throw "Error reading file " + get_file(file_path);
	}

    while (!input.eof() && processed_chars < char_cnt) {
        if (bit_index == 8) {
            bit_index = 0;
            buffer_mask = input.get();
            if (buffer_mask == -1) {
				break;
            }
		}
        bool bit = buffer_mask & (1 << (7 - bit_index));
        bit_index++;

        if (current_node == NULL) {
			output.close();
            throw (string)"Error reading file " + get_file(file_path);
		}
        if (bit) {
            current_node = current_node -> right;
        } else {
            current_node = current_node -> left;
        }
        if (current_node -> left == nullptr && current_node -> right == nullptr) {
            output << current_node -> c;
            current_node = root_;
            processed_chars++;
		}
	}
    if (processed_chars < char_cnt) {
		output.close();
        throw (string)"Error reading file " + get_file(file_path);
	}

	output.close();
}

archiver::archiver() {

}

void archiver::add_path(string path, ofstream& output) {
    root_path_ = get_path(path);
    make_file_tree(root_file_, get_path(path), get_file(path), output);
}

long long archiver::add_file(string name) {

	ifstream input(name, std::ios::binary);
	long long readed = 0;
    if (input) {
        while (!input.eof()) {
			int i = input.get();
            if (i == -1) {
				break;
            }
            frequencies_[i]++;
			readed++;
		}
	}
	input.close();
	return readed;
}

void archiver::pack(string path_from, string path_to) {
    ofstream output(path_to, std::ios::binary);
    if (!output) {
        throw (string)"Unable to create an archive at the specified path";
	}

    add_path(path_from, output);

    make_tree();
    if (root_ != NULL) {
        string s;
        build_table(root_, s);
	}

    long long char_cnt = 0, bits_number = 0;
    for (int i = 0; i < 256; i++) {
        if (frequencies_[i] != 0) {
            char_cnt++;
            bits_number += frequencies_[i] * table_[i].size();
		}
    }
    output << char_cnt << ' ';

    int bit_index = 0, buffer_mask = 0;
    if (root_ != NULL) {
        view_char(root_, output);
        long long char_count = char_cnt;
        encode_tree(root_, output, buffer_mask, bit_index, char_count);
        if (bit_index != 0) {
            output << (char)buffer_mask;
		}
	}

    bit_index = 0, buffer_mask = 0;
    files_compress(root_file_, root_path_, output, buffer_mask, bit_index);
    if (bit_index != 0) {
        output << (char)buffer_mask;
    }

	output.close();
    free();
}

void archiver::un_pack(string path, string dir)
{
	ifstream input(path, std::ios::binary);
    int char_cnt;

    if (!input) {
        throw (string)"File " + get_file(path) + " failed to open";
	}

    root_file_ = nullptr;
    if (!read_file_tree(nullptr, input)) {
        throw (string)"Unpacking error";
	}

    if (!(input >> char_cnt)) {
        throw (string)"Unpacking error";
    }
	input.get();
	string chars;

    for (int i = 0; i < char_cnt; i++) {
		int c = input.get();
        if (c == -1) {
            throw (string)"Unpacking error";
		}
		chars.push_back(c);
	}
    int buffer_mask = 0;
    int bit_index = 8;

    if (char_cnt != 0) {
        int c_index = 0;
		string code;
        read_tree(root_, code, c_index, chars, input, buffer_mask, bit_index);
    }
    bit_index = 8;
    CreateDirectoryA(dir.c_str(), 0);
    tree_decompress(root_file_, dir, input, buffer_mask, bit_index);
    free();
}

void archiver::free() {
    delete_tree(root_);
    delete_file_tree(root_file_);
    for (int i = 0; i < 256; i++) {
        frequencies_[i] = 0;
    }
}

archiver::~archiver() {
    free();
}
