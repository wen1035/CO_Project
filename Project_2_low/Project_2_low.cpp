#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;
int* Predictor(string list[], int list2[], string r[], int size);//預測器，list對應表，list2計數，r真實表，size現在位置
int two_to_ten(int L1, int L2, int L3);//2轉10

int main()
{
    string list[8] = { "SN","WN1","WN2","WN3","WT1","WT2","WT3","ST" };//對應的列表
    int list2[9] = { 0 };//計數,[9]為mis_size
    string r[100];//真實 
    int r_size = 0, str_size = 0;
    int wh_size = 0;//目前位置


    //讀檔案
    ifstream ifs("2.txt", ios::in); //檔案以輸入方式開啟(檔案資料輸入到記憶體)
    if (!ifs.is_open())//判斷
    {
        cerr << "File is not open." << endl;
        exit(1);
    }
    stringstream ss;
    ss << ifs.rdbuf();//把檔案內容寫入到ss裡
    string str(ss.str());//str複製ss值


    //r的事前準備
    for (int i = 0; i < 3; i++)
    {
        r[r_size] = "N";
        r_size++;
    }
    while (str[str_size] != NULL)//從r[3]開始把str的值複製進來
    {
        r[r_size] = str[str_size];
        r_size++;
        str_size++;
    }


    //預測
    while (wh_size < str_size)
    {
        Predictor(list, list2, r, wh_size);
        wh_size++;
    }

    ifs.close(); //開啟的檔案使用完成後關閉
    return 0;
}
//預測器，list對應表，list2計數，r真實表，size現在位置
int* Predictor(string list[], int list2[], string r[], int size)
{
    string p;//預測
    int L1 = 0, L2 = 0, L3 = 0;//看的位置
    if (r[size] == "N")
        L1 = 0;
    else
        L1 = 1;
    if (r[size + 1] == "N")
        L2 = 0;
    else
        L2 = 1;
    if (r[size + 2] == "N")
        L3 = 0;
    else
        L3 = 1;
    int L123 = 0;
    cout << "(" << L1 << L2 << L3 << ", ";
    L123 = two_to_ten(L1, L2, L3);//list2要看的位置

    //輸出當前所有位置的狀態
    for (int i = 0; i < 8; i++)
    {
        if (list2[i] == 0)
            cout << list[0] << " ";
        if (list2[i] == 1)
            cout << list[1];
        if (list2[i] == 2)
            cout << list[2];
        if (list2[i] == 3)
            cout << list[3];
        if (list2[i] == 4)
            cout << list[4];
        if (list2[i] == 5)
            cout << list[5];
        if (list2[i] == 6)
            cout << list[6];
        if (list2[i] == 7)
            cout << list[7] << " ";
        if (i == 7)
            cout << ") ";
        else
            cout << ", ";

    }
    //預測p的值
    if (list[list2[L123]] == "SN" || list[list2[L123]] == "WN1" || list[list2[L123]] == "WN2" || list[list2[L123]] == "WN3")
        p = "N";
    else
        p = "T";
    //看預測跟真實值是否一樣
    if (p != r[size + 3])
    {
        list2[8] += 1;

        cout << p << " " << r[size + 3] << "  misprediction: " << list2[8] << endl;
    }
    else
    {
        cout << p << " " << r[size + 3] << "  misprediction: " << "0" << endl;
    }
    //看真實值為T or N,來判斷下次list2[L123]的位置
    if (r[size + 3] == "T")
    {
        if (list2[L123] < 7)
            list2[L123] += 1;
    }
    else
    {
        if (list2[L123] > 0)
            list2[L123] -= 1;
    }


    return list2;
}
//2轉10
int two_to_ten(int L1, int L2, int L3)
{
    int sum = 0;
    sum = L3 * 1 + L2 * 2 + L1 * 4;
    return sum;
}