#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <cmath>
#include "ppm_to_ascii.h"
using namespace std;

const string LINE = "=============================================================================================================";

void init()
{
    cout << endl
         << LINE << endl
         << endl
         << "Welcome to image filter! (Enter -1 anywhere to end)" << endl
         << endl
         << LINE << endl;
}

void end()
{
    cout << endl
         << LINE << endl
         << endl
         << "Thank you!" << endl
         << endl
         << LINE << endl;
}

void boxfilterTrans(int **img, int **new_img, int height, int width, int filter_size, ofstream &out)
{
    // Create the kernel for box filter with filter size
    float **box_filter = new float *[filter_size];
    for (int i = 0; i < filter_size; i++)
    {
        box_filter[i] = new float[filter_size];
    }
    for (int i = 0; i < filter_size; i++)
    {
        for (int j = 0; j < filter_size; j++)
        {
            box_filter[i][j] = 1.0 / (filter_size * filter_size);
        }
    }

    // Loop through old img and put new value to new img
    float new_value = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            new_value = 0;
            for (int i = 0; i < filter_size; i++)
            {
                for (int j = 0; j < filter_size; j++)
                {
                    new_value += img[y + i][x + j] * box_filter[i][j];
                }
            }
            new_img[y][x] = int(new_value);
            out << new_img[y][x] << " ";
        }
        out << endl;
    }

    // Delete the dynamic array
    for (int i = 0; i < filter_size; i++)
    {
        delete[] box_filter[i];
    }
    delete[] box_filter;
    box_filter = NULL;
}

void constfilterTrans(int **img, int **new_img, int height, int width, int filter_size, ofstream &out)
{
    int **const_filter = new int *[filter_size];
    for (int i = 0; i < filter_size; i++)
    {
        const_filter[i] = new int[filter_size];
    }
    for (int i = 0; i < filter_size; i++)
    {
        for (int j = 0; j < filter_size; j++)
        {
            const_filter[i][j] = 0;
        }
    }
    const_filter[filter_size / 2][filter_size / 2] = 1;

    int new_value = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            new_value = 0;
            for (int i = 0; i < filter_size; i++)
            {
                for (int j = 0; j < filter_size; j++)
                {
                    new_value += img[y + i][x + j] * const_filter[i][j];
                }
            }
            new_img[y][x] = new_value;
            out << new_img[y][x] << " ";
        }
        out << endl;
    }

    for (int i = 0; i < filter_size; i++)
    {
        delete[] const_filter[i];
    }
    delete[] const_filter;
    const_filter = NULL;
}

void midValueTrans(int **img, int **new_img, int height, int width, int filter_size, ofstream &out)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            vector<int> temp;
            for (int i = 0; i < filter_size; i++)
            {
                for (int j = 0; j < filter_size; j++)
                {
                    temp.push_back(img[y + i][x + j]);
                }
            }
            sort(temp.begin(), temp.end());
            new_img[y][x] = temp[filter_size * filter_size / 2];
            out << new_img[y][x] << " ";
        }
        out << endl;
    }
}

void alphaTrimmedMean(int **img, int **new_img, int height, int width, int filter_size, int alpha, ofstream &out)
{
    int discard = alpha / 2, sum;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            vector<int> temp;
            for (int i = 0; i < filter_size; i++)
            {
                for (int j = 0; j < filter_size; j++)
                {
                    temp.push_back(img[y + i][x + j]);
                }
            }
            sort(temp.begin(), temp.end());
            sum = 0;
            for (int i = discard; i < filter_size * filter_size - discard; i++)
            {
                sum += temp[i];
            }
            // Assign the mean value to the output pixel
            new_img[y][x] = sum / (filter_size * filter_size - discard * 2);
            out << new_img[y][x] << " ";
        }
        out << endl;
    }
}

void grayStretch(int **img, int **new_img, int height, int width, int fa, int fb, int ga, int gb, ofstream &out)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {

            if (img[y][x] < fa)
            {
                new_img[y][x] = ga / fa * img[y][x];
            }
            else if (img[y][x] < fb) // fa<=img[y][x]<fb
            {
                new_img[y][x] = ga + (gb - ga) / (fb - fa) * img[y][x];
            }
            else // img[y][x] > fb
            {
                new_img[y][x] = gb + (255 - gb) / (255 - fb) * img[y][x];
            }
            out << new_img[y][x] << " ";
        }
        out << endl;
    }
}

void histogramEqualization(int **img, int **new_img, int height, int width, ofstream &out)
{
    vector<int> darkness(256);
    vector<float> cdf(256);
    vector<int> newDarkness(256);
    int minValue = 255, maxValue = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (img[y][x] < minValue)
            {
                minValue = img[y][x];
            }
            if (img[y][x] > maxValue)
            {
                maxValue = img[y][x];
            }
            darkness[img[y][x]] += 1;
        }
    }

    cdf[0] = (darkness[0] / float(height * width));
    for (int i = 1; i < 256; i++)
    {
        cdf[i] = darkness[i] / float(height * width) + cdf[i - 1];
    }
    out << endl;
    for (int i = 0; i < 256; i++)
    {
        newDarkness[i] = int(round(255 * (cdf[i] - cdf[minValue]) / (cdf[maxValue] - cdf[minValue])));
    }

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            new_img[y][x] = newDarkness[img[y][x]];
            out << new_img[y][x] << " ";
        }
        out << endl;
    }
}

int main()
{
    ifstream inFile;
    ofstream outFile;
    // ofstream asciiFile;
    string type, in_filepath, out_filepath;
    int width, height, max_value;

    /////////////////////////////////////////////////////
    int filter_size, option, fill_0, display_op;
    init();
    while (true)
    {
        cout << endl
             << "Enter the filepath (-1 to end):\n";
        cin >> in_filepath;
        inFile.open(in_filepath);
        if (in_filepath == "-1")
        {
            break;
        }

        while (inFile.fail())
        {
            cout << "Input file opening failed.\n";
            cout << endl
                 << "Enter the filepath (-1 to end):\n";
            cin >> in_filepath;
            inFile.open(in_filepath);
        }

        out_filepath = in_filepath.substr(0, in_filepath.length() - 4);
        getline(inFile, type);
        inFile >> width >> height;
        inFile >> max_value;

        cout << endl
             << "What do you want? (-1 to end)" << endl;
        cout << "(1)box filter (2)const filter (3)median filter" << endl
             << "(4)alpha-trimmed mean filter (5)gray stretch (6)histogram equalization" << endl;
        cin >> option;
        if (option == -1)
        {
            break;
        }
        while ((option < 1 || option > 6))
        {
            cout << "Not valid option." << endl;
            cout << endl
                 << "What do you want?" << endl;
            cout << "(1)box filter (2)const filter (3)median filter" << endl
                 << "(4)alpha-trimmed mean filter (5)gray stretch (6)histogram equalization" << endl;
            cin >> option;
        }

        int **new_img = new int *[height];
        for (int i = 0; i < height; i++)
        {
            new_img[i] = new int[width]();
        }

        if (option >= 1 && option <= 4)
        {
            cout << endl
                 << "Enter your filter size (-1 to end):  ";
            cin >> filter_size;
            if (filter_size == -1)
            {
                break;
            }
            while (filter_size == 0 || filter_size % 2 == 0)
            {
                cout << "Filter size must be an odd number and must not be 0." << endl;
                cout << endl
                     << "Enter your filter size: ";
                cin >> filter_size;
            }

            fill_0 = filter_size - 1;

            // initialize dynamic array/////////////////////////////////
            int **img = new int *[height + fill_0];
            for (int i = 0; i < height + fill_0; i++)
            {
                img[i] = new int[width + fill_0]();
            }

            // read original->img with zero padding/////////////////////////////////
            for (int y = 0; y < height + fill_0; y++)
            {
                for (int x = 0; x < width + fill_0; x++)
                {
                    if (y < fill_0 / 2 || x < fill_0 / 2 || y >= fill_0 / 2 + height || x >= fill_0 / 2 + width)
                    {
                        img[y][x] = 0;
                    }
                    else
                    {
                        inFile >> img[y][x];
                    }
                }
            }
            // display(img, height+fill_0, width+fill_0);

            // transform->new_img by filter////////////////////////////////
            if (option == 1)
            {
                out_filepath += "_box_" + to_string(filter_size) + ".ppm";
                outFile.open(out_filepath);
                outFile << type << endl;
                outFile << width << " " << height << endl;
                outFile << max_value << endl;
                boxfilterTrans(img, new_img, height, width, filter_size, outFile);
            }
            else if (option == 2)
            {
                out_filepath += "_const_" + to_string(filter_size) + ".ppm";
                outFile.open(out_filepath);
                outFile << type << endl;
                outFile << width << " " << height << endl;
                outFile << max_value << endl;
                constfilterTrans(img, new_img, height, width, filter_size, outFile);
            }
            else if (option == 3)
            {
                out_filepath += "_mid_" + to_string(filter_size) + ".ppm";
                outFile.open(out_filepath);
                outFile << type << endl;
                outFile << width << " " << height << endl;
                outFile << max_value << endl;
                midValueTrans(img, new_img, height, width, filter_size, outFile);
            }
            else if (option == 4) // alpha-trimmed
            {
                int alpha;
                cout << endl
                     << "Enter your alpha parameter (-1 to end): ";
                cin >> alpha;
                while (alpha % 2 == 1 || alpha >= filter_size * filter_size)
                {
                    cout << "Alpha should be even and smaller than square of filter size." << endl;
                    cout << endl
                         << "Enter your alpha parameter (-1 to end): ";
                    cin >> alpha;
                }
                if (alpha == -1)
                {
                    break;
                }

                out_filepath += "_atm" + to_string(alpha) + "_" + to_string(filter_size) + ".ppm";
                outFile.open(out_filepath);
                outFile << type << endl;
                outFile << width << " " << height << endl;
                outFile << max_value << endl;
                alphaTrimmedMean(img, new_img, height, width, filter_size, alpha, outFile);
            }

            // delete dynamic array/////////////////////////////////////////////
            for (int i = 0; i < height + fill_0; i++)
            {
                delete[] img[i];
            }
            delete[] img;
            img = NULL;
        }
        else if (option == 5) // gray stretch
        {
            out_filepath += "_graystretch.ppm";
            outFile.open(out_filepath);
            outFile << type << endl;
            outFile << width << " " << height << endl;
            outFile << max_value << endl;

            // initialize dynamic array/////////////////////////////////
            int **img = new int *[height];
            for (int i = 0; i < height; i++)
            {
                img[i] = new int[width];
            }

            // read original->img/////////////////////////////////
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    inFile >> img[y][x];
                }
            }

            int fa, fb, ga, gb;
            cout << endl
                 << "Enter your fa fb ga gb in order (0~255, spererate with space)(all -1 to end):\n";
            cin >> fa >> fb >> ga >> gb;
            if (fa == -1 && fb == -1 && ga == -1 && gb == -1)
            {
                break;
            }
            while (fa < 0 || fa > 255 || fb <= fa || fb > 255 || ga < 0 || ga > 255 || gb <= ga || gb > 255)
            {
                cout << "Value should in the range of 0~255 and fb > fa, gb > ga" << endl;
                cout << endl
                     << "Enter your fa fb ga gb in order (0-255, spererate with space):\n";
                cin >> fa >> fb >> ga >> gb;
            }

            // transform->new_img by gray stretch////////////////////////////////
            grayStretch(img, new_img, height, width, fa, fb, ga, gb, outFile);

            ///////////////////////////////////////////////////////////
            for (int i = 0; i < height; i++)
            {
                delete[] img[i];
            }
            delete[] img;
            img = NULL;
        }
        else if (option == 6) // histogram equalization
        {
            out_filepath += "_he.ppm";
            outFile.open(out_filepath);
            outFile << type << endl;
            outFile << width << " " << height << endl;
            outFile << max_value << endl;

            // initialize dynamic array/////////////////////////////////
            int **img = new int *[height];
            for (int i = 0; i < height; i++)
            {
                img[i] = new int[width];
            }

            // read original->img/////////////////////////////////
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    inFile >> img[y][x];
                }
            }

            // transform->new_img by gray stretch////////////////////////////////
            histogramEqualization(img, new_img, height, width, outFile);
            ///////////////////////////////////////////////////////////
            for (int i = 0; i < height; i++)
            {
                delete[] img[i];
            }
            delete[] img;
            img = NULL;
        }

        // hint message
        cout << endl
             << "Transfromed successful!" << endl
             << "Your new ppm filepath is \'" << out_filepath << "\'" << endl;

        // display new/////////////////////////////////////////////
        cout << endl
             << "Do you want to display your img? (-1 to end)" << endl;
        cout << "(0)don't display (1)low quality (2)mid quality (3) high quality" << endl;
        cin >> display_op;
        while (display_op != -1 && (display_op < 0 || display_op > 3))
        {
            cout << "Not valid option." << endl;
            cout << endl
                 << "Do you want to display your img? (-1 to end)" << endl;
            cout << "(0)don't display (1)low quality (2)mid quality (3) high quality" << endl;
            cin >> display_op;
        }
        if (display_op == -1)
        {
            break;
        }
        else if (display_op == 1)
        {
            display(new_img, height, width);
        }
        else if (display_op == 2)
        {
            displayMQ(new_img, height, width);
        }
        else if (display_op == 3)
        {
            displayHQ(new_img, height, width);
        }

        for (int i = 0; i < height; i++)
        {
            delete[] new_img[i];
        }
        delete[] new_img;
        new_img = NULL;

        // asciiFile.close();
        inFile.close();
        outFile.close();

        cout << endl
             << LINE << endl
             << endl;
    }

    end();

    /////////////////////////////////////////
    return 0;
}