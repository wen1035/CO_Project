#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;
int* Predictor(string list[], int list2[], string r[], int size);//�w�����Alist������Alist2�p�ơAr�u���Asize�{�b��m
int two_to_ten(int L1, int L2, int L3);//2��10

int main()
{
    string list[8] = { "SN","WN1","WN2","WN3","WT1","WT2","WT3","ST" };//�������C��
    int list2[9] = { 0 };//�p��,[9]��mis_size
    string r[100];//�u�� 
    int r_size = 0, str_size = 0;
    int wh_size = 0;//�ثe��m


    //Ū�ɮ�
    ifstream ifs("2.txt", ios::in); //�ɮץH��J�覡�}��(�ɮ׸�ƿ�J��O����)
    if (!ifs.is_open())//�P�_
    {
        cerr << "File is not open." << endl;
        exit(1);
    }
    stringstream ss;
    ss << ifs.rdbuf();//���ɮפ��e�g�J��ss��
    string str(ss.str());//str�ƻsss��


    //r���ƫe�ǳ�
    for (int i = 0; i < 3; i++)
    {
        r[r_size] = "N";
        r_size++;
    }
    while (str[str_size] != NULL)//�qr[3]�}�l��str���Ƚƻs�i��
    {
        r[r_size] = str[str_size];
        r_size++;
        str_size++;
    }


    //�w��
    while (wh_size < str_size)
    {
        Predictor(list, list2, r, wh_size);
        wh_size++;
    }

    ifs.close(); //�}�Ҫ��ɮרϥΧ���������
    return 0;
}
//�w�����Alist������Alist2�p�ơAr�u���Asize�{�b��m
int* Predictor(string list[], int list2[], string r[], int size)
{
    string p;//�w��
    int L1 = 0, L2 = 0, L3 = 0;//�ݪ���m
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
    L123 = two_to_ten(L1, L2, L3);//list2�n�ݪ���m

    //��X��e�Ҧ���m�����A
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
    //�w��p����
    if (list[list2[L123]] == "SN" || list[list2[L123]] == "WN1" || list[list2[L123]] == "WN2" || list[list2[L123]] == "WN3")
        p = "N";
    else
        p = "T";
    //�ݹw����u��ȬO�_�@��
    if (p != r[size + 3])
    {
        list2[8] += 1;

        cout << p << " " << r[size + 3] << "  misprediction: " << list2[8] << endl;
    }
    else
    {
        cout << p << " " << r[size + 3] << "  misprediction: " << "0" << endl;
    }
    //�ݯu��Ȭ�T or N,�ӧP�_�U��list2[L123]����m
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
//2��10
int two_to_ten(int L1, int L2, int L3)
{
    int sum = 0;
    sum = L3 * 1 + L2 * 2 + L1 * 4;
    return sum;
}