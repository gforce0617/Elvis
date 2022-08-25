#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <deque>
#include <thread>
#include <mutex>

using namespace std;

mutex mx;

deque<string> split_str(vector<string>& vec);
void processing_file(string& path_files, deque<string> &result);
void save_words(deque<string>& result, string o_path);

template<class T>
void print_vector(vector<T>& vec)
{
    for (auto el : vec)
    {
        cout << el << endl;
    }
}

template<class T>
void print_deque(deque<T>& deq)
{
    for (auto el : deq)
    {
        cout << el << endl;
    }
}

void processing_file(string &path_files, deque<string> &result)
{
    ifstream fin;
    fin.exceptions(ifstream::badbit | ifstream::failbit);
   
    try
    {
        fin.open(path_files);
        
    }
    catch (const std::exception& ex)
    {
        cout << "Ошибка открытия" << endl;
        cout << ex.what() << endl;
    }

    vector<string> str;
    string buff;

    while (!fin.eof())
    {
        getline(fin, buff);
        str.push_back(buff);
    }

    fin.close();

    //print_vector(str);

    deque<string> split_words = split_str(str);
    str.clear();

    split_words.push_front(path_files);

    mx.lock();
    for (auto el : split_words)
    {
        result.push_back(el);
    }
    mx.unlock();

}


deque<string> split_str(vector<string>& vec)
{
    deque<string> elems;
    string temp = "";
    int count;

    for (int i = 0; i <= vec[0].size(); i++) {

        //Сравнение с разделителями
        count = 0;
        for (int j = 1; j < vec.size(); j++)
        {
            if (vec[0][i] == vec[j][0]) {
                count++;
                break;
            }
        }

        if ((count != 0) || (i == vec[0].size()))
        {
            if (temp != "")
                elems.push_back(temp);
            temp = "";
        }
        else
        {
            temp += vec[0][i];
        }

    }

    return elems;
}

void save_words(deque<string>& result, string o_path)
{
    ofstream fout;

    fout.exceptions(ofstream::badbit | ofstream::failbit);

    try
    {
        fout.open(o_path);
    }
    catch (const std::exception& ex)
    {
        cout << "Ошибка открытия" << endl;
        cout << ex.what() << endl;
    }

    for (auto elem : result)
    {
        fout << elem << endl;
    }
    fout.close();
}



int main()
{
    setlocale(LC_ALL, "ru");
    string i_path;
    string o_path;
    vector<string> path_files;
    vector<thread> th;

    cout << "Введите путь к каталогу с файлами" << endl;
    cin >> i_path;
    cout << "Введите путь к результирующему файлу" << endl;
    cin >> o_path;

    //Получение путей ко всем файлам в каталоге
    for (const auto& entry : filesystem::directory_iterator(i_path))
    {
        path_files.push_back(entry.path().string());
    }
    //print_vector(path_files);

    deque<string> result;
    //Создание потоков для обработки файлов
    for (int i = 0; i < path_files.size(); ++i)
    {
        th.push_back(thread(processing_file, ref(path_files[i]), ref(result)));
    }

    for (int i = 0; i < path_files.size(); ++i)
    {
        th.at(i).join();
    }
    
    //print_deque(result);

    save_words(result, o_path);

    result.clear();
    path_files.clear();
    return 0;
}
