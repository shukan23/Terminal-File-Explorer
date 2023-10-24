// Includes

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <iostream>
#include <stdint.h>
#include <pwd.h>
#include <grp.h>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <errno.h>
#include <algorithm>
#include <termios.h>
#include <stack>
#include <cstdio>
#include <fstream>
#include <stdlib.h>
#include <fcntl.h>

using namespace std;

// Function Definations

void HavePathAndPrint(string);
void printScreen();
void printScreenCM();

void errorAndExit(const char *);
int calculateWinRow();
void goTOCanonical();
void goToNonCanonical();
string getUserName();

void go_back_leftkey();
void go_forward_rightkey();
void enterPressed();

int calculateWinRow();
void clearForwardStack();
string getParentPath();
void goToNormalMode();
void goToCommandMode();
void HavePathAndPrint(string);
string calculateAbsPath(string);
void goToNonCanonicalForCM();
unsigned int calculateWinCols();
bool existsInSrc(string,string);


void searchTheELement(string, string);

void renameFile(string, string);

void CreateNewFile(string, string);
void CreateNewFolder(string, string);

void copyFileOrDir(string, string, string);
void copyFile(string, string, string);
void copyDirectory(string, string, string);

void deleteFile(string);
void deleteDirectory(string);

void moveFileOrDir(string,string, string);
void moveFile(string, string, string);
void moveDirectory(string, string, string);

//  Global Variables
struct termios original_trs;
int len;
int maxWdOfName;
int cursor, fstart, fend;
int windowRows;
stack<string> backPathTracker;
stack<string> forwardPathTracker;
string currPath;
string  **elementsOfPrint;
bool firstTimeNM;
bool want_to_exit;
bool searchFound;
bool ModeOfSystem; // True for Normal Mode and False for Command Mode

// Normal Mode

void HavePathAndPrint(string path)
{
    DIR *dirref;

    // cout<<"2\n";
    // if(opendir(path) == -1 ) errorAndExit("Not Access");
    for (int i = 0; i < len; i++)
    {
        delete[] elementsOfPrint[i];
    }
    len = 0;
    cursor = 0;
    fstart = 0;
    maxWdOfName = 15;

    dirref = opendir(path.c_str());
    // cout<<"3\n";
    
    if (dirref == NULL)
    {
        cout << strerror(errno);
    }

    

    struct dirent *refToElement;
    struct stat Statistics1;
    struct passwd *CurrentOwner;
    struct group *CurrentGroup;
    // cout << "4\n";
    refToElement = readdir(dirref);
    // cout << "5\n";

    string names[500];
    while (refToElement != NULL)
    {
        names[len] = refToElement->d_name;
        refToElement = readdir(dirref);
        if(maxWdOfName < names[len].size()){
            maxWdOfName = names[len].size();
        }
        len++;
        
    }

    sort(names, names + len);

    for (int i = 0; i < len; i++)
    {

        elementsOfPrint[i] = new string[6];
        // cout << "6\n";

        string dirPath = "";
        dirPath = path + "/";
        dirPath += names[i];
        stat(dirPath.c_str(), &Statistics1);

        // Store The File Size at Col 0
        // cout<<"7\n";
        // cout<<"orignal Val : "<<Statistics1.st_size<<endl;
        long long int s = (Statistics1.st_size) / 1024;
        // cout<<"Integer : "<<s<<endl;
        string size = to_string(s);
        // cout<<"String : "<<size<<endl;
        elementsOfPrint[i][0] = size + "KB";
        // cout<<"7.1\n";

        // Store The User Owner Name at Col 1

        CurrentOwner = getpwuid(Statistics1.st_uid);
        cout << "7.2\n";
        if (CurrentOwner != NULL)
        {
            // cout<<(pw->pw_name);
            elementsOfPrint[i][1] = (string)(CurrentOwner->pw_name);
        }

        // cout << "8\n";
        // Store The Group Name at Col 2
        CurrentGroup = getgrgid(Statistics1.st_gid);
        if (CurrentGroup != NULL)
        {
            elementsOfPrint[i][2] = CurrentGroup->gr_name;
        }

        // cout<<"8.1\n";
        // Store The Permissions at Col 3
        elementsOfPrint[i][3] = "";
        if ((S_ISDIR(Statistics1.st_mode)))
        {
            elementsOfPrint[i][3] = elementsOfPrint[i][3] + "d";
        }
        else
        {
            elementsOfPrint[i][3] = elementsOfPrint[i][3] + "-";
        }
        // cout<<"9\n";
        elementsOfPrint[i][3] += (Statistics1.st_mode & S_IRUSR) ? "r" : "-";
        elementsOfPrint[i][3] += (Statistics1.st_mode & S_IWUSR) ? "w" : "-";
        elementsOfPrint[i][3] += (Statistics1.st_mode & S_IXUSR) ? "x" : "-";
        elementsOfPrint[i][3] += (Statistics1.st_mode & S_IRGRP) ? "r" : "-";
        elementsOfPrint[i][3] += (Statistics1.st_mode & S_IWGRP) ? "w" : "-";
        elementsOfPrint[i][3] += (Statistics1.st_mode & S_IXGRP) ? "x" : "-";
        elementsOfPrint[i][3] += (Statistics1.st_mode & S_IROTH) ? "r" : "-";
        elementsOfPrint[i][3] += (Statistics1.st_mode & S_IWOTH) ? "w" : "-";
        elementsOfPrint[i][3] += (Statistics1.st_mode & S_IXOTH) ? "x" : "-";

        // Store The Lase Modified at Col 4
        // cout<<ctime(&Statistics1.st_mtime)<<"\n";
        string timeWithNew = (string)ctime(&Statistics1.st_mtime);
        timeWithNew[timeWithNew.size() - 1] = '\0';
        elementsOfPrint[i][4] = timeWithNew.substr(4, 12);

        // Store The Filename at Col 5
        elementsOfPrint[i][5] = names[i];
    }
    // Close the directory and returning
    closedir(dirref);

    // Ptint
    if (ModeOfSystem)
    {
        printScreen();
    }
    else
    {
        printScreenCM();
    }

}

void printScreen()
{
    printf("\33c");
    
    fend = fstart + calculateWinRow();
    int count = calculateWinRow();
    if (fend >= len)
    {
        fstart = 0;
        fend = fstart + calculateWinRow();
    }

    // LOGIC to Handle Width
    bool colFlag[6] = {true,true,true,true,true,true};
    unsigned int NoOFCols = calculateWinCols();
    int indexWd = log10(len)+1;
    if(NoOFCols < (indexWd+maxWdOfName+58)){
        colFlag[1]=false;
        colFlag[2] = false;
         if(NoOFCols < (indexWd+maxWdOfName+42)){
            colFlag[3]=false;
            colFlag[4] = false;
         }
    }


    for (int j = 0; j < len; j++)
    {

        if (j >= fstart && j <= fend)
        {
            string str = (j == cursor)  ? "--> " : "    ";
            if (j == cursor) cout<<"\033[7;37m";
            cout << str;
            cout<<"["<<right<< setw(indexWd)<<(j+1);
            cout<<"]";
            if(colFlag[0])   cout << right << setw(8) << elementsOfPrint[j][0] << "  ";     // size of file
            if(colFlag[1])   cout << left << setw(8) << elementsOfPrint[j][1];              // Owner Name
            if(colFlag[2])   cout << left << setw(8) << elementsOfPrint[j][2];              // Group Name
            if(colFlag[3])   cout << left << elementsOfPrint[j][3] << "  ";                 // Permissions
            if(colFlag[4])   cout << elementsOfPrint[j][4] << "  ";                         // Modified time
            if(colFlag[5])   cout << left<<setw(15)<< elementsOfPrint[j][5];             // Name of the file
            if (j == cursor) cout<<"\033[0m";
            // cout<<elementsOfPrint[j][6]<<setw();
            cout << endl;
            count--;
        }
    }
    while (count >= 0)
    {
        cout << endl;
        count--;
    }
    cout << endl<< "PATH :: " << currPath << endl ;
    cout << "\033[1;32mNORMAL MODE \033[0m"<< endl;

    
}

void go_back_leftkey()
{
    if (backPathTracker.empty())
    {
        return;
    }
    else
    {
        forwardPathTracker.push(currPath);
        currPath = backPathTracker.top();
        backPathTracker.pop();
        HavePathAndPrint(currPath);
    }
}

void enterPressed(){
    
    string absp = currPath +"/"+elementsOfPrint[cursor][5];
    struct stat Statistics2;
    stat(absp.c_str(),&Statistics2);
    
    if((S_ISDIR(Statistics2.st_mode))){

        DIR* dir = opendir(absp.c_str());
        if(errno == EACCES){
            cout << endl << "Permissions denied for read......" << endl;
            return ;
        }

        backPathTracker.push(currPath);
        clearForwardStack();
        string NewPath;
        NewPath = currPath;
        if (NewPath != "/")
            NewPath = NewPath + "/";
        NewPath = NewPath + elementsOfPrint[cursor][5];
        currPath = NewPath;
        HavePathAndPrint(currPath);
    }
    else{
        pid_t NewChildProcess;
        NewChildProcess = fork();
        if(NewChildProcess == 0){
            execl("/usr/bin/xdg-open", "xdg-open",absp.c_str() , NULL);
            exit(1);
        }
    }
}

void go_forward_rightkey()
{
    if (forwardPathTracker.empty())
    {
        return;
    }
    else // if (rightPathTracker. <  rightPathTracker.top()) {
    {
        backPathTracker.push(currPath);      // Changed at last
        currPath = forwardPathTracker.top();
        forwardPathTracker.pop();
        HavePathAndPrint(currPath);
    }
}

int calculateWinRow()
{
    struct winsize scws;
    ioctl(0, TIOCGWINSZ, &scws);
    windowRows = scws.ws_row - 6;
    return windowRows;

    // printf ("lines %d\n", w.ws_row);
    // printf ("columns %d\n", w.ws_col);
}

unsigned int calculateWinCols()
{
    struct winsize scws;
    ioctl(0, TIOCGWINSZ, &scws);
    unsigned int windowCols = scws.ws_col;
    return windowCols;

    // printf ("lines %d\n", w.ws_row);
    // printf ("columns %d\n", w.ws_col);
}

void goToNormalMode()
{

    ModeOfSystem = true;

    char inp;
    int readStatus;

    goToNonCanonical();

    if (firstTimeNM)
    {
        string NewPath = getUserName();
        NewPath = "/home/" + NewPath;
        currPath = NewPath;
        firstTimeNM = false;
    }
    HavePathAndPrint(currPath);

    while (readStatus = (read(STDIN_FILENO, &inp, 1)) == 1)
    {

        if (readStatus == -1 && errno != EAGAIN)
        {
            errorAndExit("Error While Reading");
        }

        if (inp == '\x1b')
        {                   // If Escape sequence is detected
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) != 1)
                continue;
            if (read(STDIN_FILENO, &seq[1], 1) != 1)
                continue;
            if (seq[0] == '[')
            {
                switch (seq[1])
                {
                case 65:
                    if (cursor > 0) // return ARROW_UP;
                    {
                        cursor--;
                        if (cursor == fstart-1)
                        {
                            fstart--;
                        }
                        printScreen();
                    }
                    break;
                case 66: // return ARROW_DOWN;
                    if (cursor < len - 1)
                    {
                        cursor++;
                        if (cursor == fend+1)
                        {
                            //fstart = fend - calculateWinRow() + 1 ;
                            fstart++;
                        }
                        printScreen();
                    }
                    break;
                case 67:
                    go_forward_rightkey(); // return ARROW_RIGHT;
                    break;
                case 68:
                    go_back_leftkey(); // return ARROW_LEFT;
                    break;
                }
            }
        }

        else if (inp == 'q')
        {
            want_to_exit = true;
            break;
        }

        else if (inp == 10)
        {               // input is Enter

            if (elementsOfPrint[cursor][5] != ".")
            {
                if (elementsOfPrint[cursor][5] == "..")
                {
                    goto backspace;
                }
                else
                {
                    enterPressed(); 
                }
            }
        }

        else if (inp == 127)
        { // if backspace is detected
        backspace:
            if (currPath != "/")
            {
                backPathTracker.push(currPath);
                clearForwardStack();
                currPath = getParentPath();
                HavePathAndPrint(currPath);
            }
        }

        else if (inp == 'h')
        {
            backPathTracker.push(currPath);
            // ? clear fwd stack
            clearForwardStack();
            string uname = getUserName();
            string NewPath = "/home/" + uname;
            currPath = NewPath;
            HavePathAndPrint(NewPath);
        }

        else if (inp == ':')
        {
            break;
        }
    }
    goTOCanonical();
}

// Command Mode

void printScreenCM()
{
    printf("\33c");

    // fend = fstart + calculateWinRow();
    int count = calculateWinRow();
    int slimit = count;
    // if(fend >= len){
    //     fstart=0;
    //     fend = len-1;
    // }

    // LOGIC to Handle Width
    bool colFlag[6] = {true,true,true,true,true,true};
    unsigned int NoOFCols = calculateWinCols();
    int indexWd = log10(len)+1;
    if(NoOFCols < (indexWd+maxWdOfName+58)){
        colFlag[1]=false;
        colFlag[2] = false;
         if(NoOFCols < (indexWd+maxWdOfName+42)){
            colFlag[3]=false;
            colFlag[4] = false;
         }
    }

    for (int j = 0; j < len && j < slimit; j++)
    {
        // string str = j == cursor ? "-->" : "   ";
        cout<<"    ["<<right<< setw(indexWd)<<(j+1);
        cout<<"]";
        cout << right << setw(8) << elementsOfPrint[j][0] << "  ";
        cout << left << setw(8) << elementsOfPrint[j][1];
        cout << left << setw(8) << elementsOfPrint[j][2];
        cout << left << elementsOfPrint[j][3] << "  ";
        cout << elementsOfPrint[j][4] << "  ";
        cout << left << setw(15) << elementsOfPrint[j][5];

        // cout<<elementsOfPrint[j][6]<<setw();
        cout << endl;
        count--;
    }
    while (count >= 0)
    {
        cout << endl;
        count--;
    }
    cout << endl << "PATH :: " << currPath << endl;
    write(STDOUT_FILENO, "\033[1;37mCOMMAND MODE :: $\033[0m ", 29);
    // cout << endl<<"COMMAND MODE :: $\t ";
    
}

string calculateAbsPath(string path)
{
    string Abs;
    char finalpath[1024];
    int psize = path.size();
    if (path[0] == '.')
    {
        Abs = currPath + "/" + path;
    }
    else if (path[0] == '~')
    {
        Abs = "/home/" + getUserName() + "/" + path.substr(1, psize - 1);
    }
    else if (path[0] == '/'){
        Abs = path;
    }
    else
    {
        Abs = currPath + "/" + path;
    }
    //  return Abs;
    if (realpath(Abs.c_str(), finalpath) == NULL)
    {
        cout << strerror(errno) << endl;
        return NULL;
    }
    else
    {
        string Fpath(finalpath);
        return Fpath;
    }
}

void searchTheELement(string element, string path)
{
    if (element != "." && element != "..")
    {
        path = calculateAbsPath(path);
        DIR *thisdir;
        // cout << "came to search at : " << path << endl;
        thisdir = opendir(path.c_str());
        if (thisdir == NULL)
        {
            cout<<strerror(errno);
            searchFound = false;
            return;
        }
        else
        {
            struct dirent *eleInPath;
            eleInPath = readdir(thisdir);
            while (eleInPath != NULL && !(searchFound))
            {   
                string sName  = eleInPath->d_name;
                if (sName == element)
                {
                    searchFound = true;
                    closedir(thisdir);
                    return;
                }
                else if (eleInPath->d_type == DT_DIR)
                {
                    if (strcmp(eleInPath->d_name, ".") != 0 && strcmp(eleInPath->d_name, "..") != 0)
                    {
                        string NewPath = path;
                        NewPath += "/";
                        NewPath += sName;
                        searchTheELement(element, NewPath);
                    }
                }
                eleInPath = readdir(thisdir);
            }
            closedir(thisdir);
        }
    }
}

void renameFile(string oldName, string newName)
{
    string old = currPath + "/" + oldName;
    string newone = currPath + "/" + newName;
    if (rename(old.c_str(), newone.c_str()) != 0)
    {
        cout << strerror(errno);
    }
    else
    {
        HavePathAndPrint(currPath);
    }
}

void CreateNewFile(string name, string path)
{

    string absp = path + "/" + name;
    int status = open(absp.c_str(), O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);        // Default Permissions -rw-rw-r--
    if (status == -1)
    {
        cout <<"Could Not Create !"<< endl;
        cout << strerror(errno) << endl;
        return;
    }
    cout<<"File Created"<<endl;
    close(status);
}

void CreateNewFolder(string fname, string path)
{
    int status;
    string absp = path + "/" + fname;
    status = mkdir(absp.c_str(), 0775);     // Here 0770 means first 0 says octal then bits for rwx rwx rwx to give permission to new folder
    if (status == 0)
    {
        cout<< "Folder Created Successfully" << endl;
    }
    else
    {
        cout<<"Folder Not Created"<<endl;
        cout << strerror(errno) << endl;
    }
}

void copyFileOrDir(string srcPath, string fname, string destPath)
{
    destPath = calculateAbsPath(destPath);

    // Check fname presents in current or not 
    if(existsInSrc(fname,srcPath)){    // Treat it as name and handle

        struct stat Statistics3;
        string toFname = srcPath + "/" + fname;
        stat(toFname.c_str(), &Statistics3);

        if ((S_ISDIR(Statistics3.st_mode))){
            // It is Directory
            // cout<<"It is DIr"<<endl;
            copyDirectory(srcPath, fname, destPath);
        }
        else{   // It is File
            // cout<<"It is File"<<endl;
            copyFile(srcPath, fname, destPath);
        }
    }
    else{     // Specified is path and handle it
        
        fname = calculateAbsPath(fname);
        if(fname.size() <= 0){
            cout<<"Source is Not Well Defined"<<endl;
        }
        else{
            int n = fname.size();
            int i = n-1;
            while(fname[i] != '/'){
                i--;
            }
            string newName = fname.substr(i+1,n-i-1);
            string newSource = fname.substr(0,i+1);
            // cout<<"New Src is : "<<newSource<<endl;
            // cout<<"Name is : "<<newName<<endl;
            // cout<<"Going to call ... "<<endl;
            copyFileOrDir(newSource,newName,destPath);
        }
    }
    return;
}

void copyFile(string srcPath, string fname, string destPath)
{
    srcPath = calculateAbsPath(srcPath);
    destPath = calculateAbsPath(destPath);
    string input = srcPath + "/" + fname;
    string output = destPath + "/" + fname;

    struct stat Statistics3;
    stat(input.c_str(), &Statistics3);

    int status1, status2;

    status1 = open(input.c_str(), O_RDONLY);
    status2 = open(output.c_str(), O_CREAT | O_WRONLY | O_TRUNC, Statistics3.st_mode);

    if (status1 == -1 || status2 == -1)
    {
        cout<<"Could not Copy";
        cout << strerror(errno) << endl;
        sleep(3);
    }
    char databyte;
    while (read(status1, &databyte, 1))
    {
        write(status2, &databyte, 1);
    }
    cout<<"File Copied .. "<<endl;
    close(status1);
    close(status2);
}

void copyDirectory(string srcPath, string dname, string destPath)
{
    // Permission for current Folder
    srcPath = calculateAbsPath(srcPath);
    destPath = calculateAbsPath(destPath);
    if (dname != "." && dname != "..")
    {
        string toOpendir = srcPath + "/" + dname;

        struct stat Statistics3;
        stat(toOpendir.c_str(), &Statistics3);

        // Create folder with existing permissions
        int status;
        string toCreateDirectory = destPath + "/" + dname;
        status = mkdir(toCreateDirectory.c_str(), Statistics3.st_mode);
        if (status != 0)
        {
            return;
        }
        // cout << "Dir Created at : " << toCreateDirectory << endl;
        DIR *dirref;

        dirref = opendir(toOpendir.c_str());
        struct stat Statistics4;
        struct dirent *refToElement;

        refToElement = readdir(dirref);
        while (refToElement != NULL)
        {

            string elementName = refToElement->d_name;
            string dirPath = toOpendir + "/" + elementName;
            stat(dirPath.c_str(), &Statistics4);

            if ((S_ISDIR(Statistics4.st_mode)) && elementName != ".." && elementName != ".")
            {
                string newSource = srcPath + "/" + dname;
                string newDestination = destPath + "/" + dname;
                copyDirectory(newSource, elementName, newDestination);
            }
            else if (elementName != ".." && elementName != ".")
            {
                string newSource = srcPath + "/" + dname;
                string newDestination = destPath + "/" + dname;
                copyFile(newSource, elementName, newDestination);
                // cout << "File Copied from : " << newSource << "/" << elementName << endl;
            }

            refToElement = readdir(dirref);
        }
        closedir(dirref);
    }
}

void deleteFile(string path)
{
    path = calculateAbsPath(path);
    int status = unlink(path.c_str());
    // int status = 0 ;
    if (status == 0)
    { // On successful deletion
        // cout << "File deleted at : " << path << endl;
        // sleep(3);
    }
    else
    {
        cout << strerror(errno) << endl;
        sleep(3);
    }
}

void deleteDirectory(string path)
{
    path = calculateAbsPath(path);
    if (path != "." && path != "..")
    {
        DIR *dirref;
        dirref = opendir(path.c_str());

        struct dirent *refToElement;
        struct stat Statistics4;

        refToElement = readdir(dirref);
        while (refToElement != NULL)
        {
            string NameOfElement = refToElement->d_name;
            string dirPath = path + "/" + NameOfElement;
            stat(dirPath.c_str(), &Statistics4);

            if ((S_ISDIR(Statistics4.st_mode)) && (NameOfElement != "..") && (NameOfElement != "."))
            {
                string newPathForDelete = path + "/" + NameOfElement;
                deleteDirectory(newPathForDelete);
            }
            else if ((NameOfElement != "..") && (NameOfElement != "."))
            {
                string newPathForDelete = path + "/" + NameOfElement;
                // cout<<"File deleted at : "<<newPathForDelete<<endl;
                deleteFile(newPathForDelete);
            }
            refToElement = readdir(dirref);
            sleep(1);
        }
        closedir(dirref);
        // rmdir(path.c_str());
        // cout << "--------------------------   Directory Deleted : " << path << "-----------------------------" << endl;
        //    delete the directory itself
    }
}

void moveFileOrDir(string srcPath, string eleName, string destPath)
{
    destPath = calculateAbsPath(destPath);
    srcPath = calculateAbsPath(srcPath);
    if(existsInSrc(eleName,srcPath)){
        struct stat Statistics3;
        string toFname = srcPath + "/" + eleName;
        calculateAbsPath(toFname);
        stat(toFname.c_str(), &Statistics3);
        if ((S_ISDIR(Statistics3.st_mode)))
        {
            // It is Directory
            // cout<<"It is DIr"<<endl;

            moveDirectory(srcPath, eleName, destPath);
            // copyDirectory(srcPath,fname,destPath);
        }
        else
        {
            // It is File
            // cout<<"It is File"<<endl;
            moveFile(srcPath, eleName, destPath);
        }
    }
    else{
        eleName = calculateAbsPath(eleName);
        if(eleName.size() <= 0){
            cout<<"Source is Not Well Defined"<<endl;
            sleep(2);
        }
        else{
            int n = eleName.size();
            int i = n-1;
            while(eleName[i] != '/'){
                i--;
            }
            string newName = eleName.substr(i+1,n-i-1);
            string newSource = eleName.substr(0,i+1);
            cout<<"New Src is : "<<newSource<<endl;
            cout<<"Name is : "<<newName<<endl;
            cout<<"Going to call ... "<<endl;
            moveFileOrDir(newSource,newName,destPath);
        }
    }

    return;
}

void moveDirectory(string srcPath, string eleName, string destPath)
{
    copyDirectory(srcPath, eleName, destPath);
    string deletePath = srcPath + "/" + eleName;
    deletePath = calculateAbsPath(deletePath);  
    cout << "Dir is moved from : " << deletePath << endl;
    sleep(1);
    deleteDirectory(deletePath);
}

void moveFile(string srcPath, string fname, string destPath)
{
    copyFile(srcPath, fname, destPath);
    string deletePath = srcPath + "/" + fname;
    cout << "File is moved from : " << deletePath << endl;
    sleep(1);
    deleteFile(deletePath);
}

void goToCommandMode()
{

    ModeOfSystem = false;

    goToNonCanonicalForCM();

    HavePathAndPrint(currPath);

    char inp;
    int readStatus;
    string inputfull[300];
    string maininput;
    int sizeOfInput = 0;
    // string temp[3];
    int j;
    j = -1;
NextCommand:
    maininput ="";
    readStatus = 0;
    sizeOfInput = 0;
    for (int i = 0; i <= j; i++)
    {
        inputfull[i] = "";
    }
    j = 0;
    while (readStatus = (read(STDIN_FILENO, &inp, 1)) == 1)
    {
        if (readStatus == -1 && errno != EAGAIN)
        {
            errorAndExit("Error While Reading");
        }
        if (inp == 10)
        { // Enter -- execute the input
            break;
        }
        else if (inp == 27)
        { // ESC go to normal Mode
            goTOCanonical();
            return;
        }
        else if(inp == 127){
            
            if(inputfull[j] == ""){
                continue;
            }else{
                inputfull[j].pop_back();
                write(STDOUT_FILENO,"\b\b\b",3);
            }
        // if(!inputfull[j].empty()){
        //     inputfull[j].pop_back();
        //     cout<<"\b \b";
        // }
        }
        else if (inp == ' ')
        {
            if (inputfull[j].size() > 0)
            {
                j++;
            }
        }
        else
        {
            inputfull[j] += inp;
        }
    }

    if (inputfull[0] == "quit")
    {
        want_to_exit = true;
        return;
    }
    if (inputfull[0] == "goto")
    {
        string NewPath = calculateAbsPath(inputfull[1]);

        if(NewPath.size() == 0){
            cout <<"Invalid Path !"<< endl;
            sleep(2);
            HavePathAndPrint(currPath);
        }
        else{
            DIR *dirref;

            if (opendir(NewPath.c_str()) == NULL)
            {
                cout <<"Invalid Path !"<< endl;
                sleep(2);
                HavePathAndPrint(currPath);
            }
            else
            {
                backPathTracker.push(currPath);
                currPath = NewPath;
                clearForwardStack();
                cout<<"Going to ..."<<endl;
                sleep(2);
                HavePathAndPrint(NewPath);
            }
        }
        
    }
    if (inputfull[0] == "search")
    {
        searchFound = false;
        searchTheELement(inputfull[1], currPath);
        if (searchFound)
        {
            cout << " True" << endl;
        }
        else
        {
            cout  << " False" << endl;
        }
        sleep(2);
    }
    if (inputfull[0] == "rename")
    {
        renameFile(inputfull[1], inputfull[2]);
    }
    if (inputfull[0] == "create_file")
    {
        inputfull[2] = calculateAbsPath(inputfull[2]);
        CreateNewFile(inputfull[1], inputfull[2]);
        sleep(2);
    }
    if (inputfull[0] == "create_dir")
    {
        inputfull[2] = calculateAbsPath(inputfull[2]);
        CreateNewFolder(inputfull[1], inputfull[2]);        // What if path is empty 
        sleep(2);
    }
    if (inputfull[0] == "copy")
    {
        inputfull[j] = calculateAbsPath(inputfull[j]);
        for (int i = 1; i < j; i++)
        {
            copyFileOrDir(currPath, inputfull[i], inputfull[j]);
        }
    }
    if (inputfull[0] == "delete_file")
    {
        // inputfull[1] = calculateAbsPath(inputfull[1]);
        deleteFile(inputfull[1]);
        HavePathAndPrint(currPath);
    }
    if (inputfull[0] == "delete_dir")
    {
        inputfull[1] = calculateAbsPath(inputfull[1]);
        deleteDirectory(inputfull[1]);
    }
    if (inputfull[0] == "move")
    {
        inputfull[j] = calculateAbsPath(inputfull[j]);
        for (int i = 1; i < j; i++)
        {
            moveFileOrDir(currPath, inputfull[i], inputfull[j]);
        }
    }
    HavePathAndPrint(currPath);
    // sizeOfInput = inputfull.size();
    // for(int i=0 ; i<sizeOfInput ; i++){
    //     if(inputfull[i] == ' '){
    //         j++;
    //     }
    //     temp[j]+= inputfull[i];
    // }
    // sleep(3);
    // HavePathAndPrint(currPath);
    goto NextCommand;
}

//  General Purpose Functions

void goTOCanonical()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_trs) == -1)
    {
        errorAndExit("ErrorAtGoTOCanonical");
    };
}

void errorAndExit(const char *message)
{
    perror(message);
    exit(1);
}

void goToNonCanonical()
{

    if (tcgetattr(STDIN_FILENO, &original_trs) == -1)
    {
        errorAndExit("ErrorAtgoToNonCanonicalAttcgetattr");
    }

    atexit(goTOCanonical);

    struct termios trs = original_trs;
    trs.c_lflag &= ~(ECHO | ICANON);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &trs))
    {
        errorAndExit("ErrorAtgoToNonCanonicalAttcsetattr");
    }
}

void goToNonCanonicalForCM()
{

    if (tcgetattr(STDIN_FILENO, &original_trs) == -1)
    {
        errorAndExit("ErrorAtgoToNonCanonicalAttcgetattr");
    }

    atexit(goTOCanonical);

    struct termios trs = original_trs;
    trs.c_lflag &= ~(ICANON);
    // trs.c_oflag &= ~(OPOST);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &trs))
    {
        errorAndExit("ErrorAtgoToNonCanonicalAttcsetattr");
    }
}

string getUserName()
{
    struct stat Statistics2;
    struct passwd *pws1;
    string NewPath = "";
    string currentPath = ".";
    stat(currentPath.c_str(), &Statistics2);
    pws1 = getpwuid(Statistics2.st_uid);
    if (pws1 != NULL)
    {
        // cout<<(pw->pw_name);
        NewPath = (string)(pws1->pw_name);
    }
    return NewPath;
}

void clearForwardStack()
{
    while (!forwardPathTracker.empty())
    {
        forwardPathTracker.pop();
    }
}

string getParentPath()
{
    string NewPath;
    int i = currPath.size() - 1;
    while (currPath[i] != '/')
    {
        i--;
    }

    NewPath = currPath.substr(0, i);
    if (NewPath == "")
        NewPath = "/";

    return NewPath;
}

bool existsInSrc(string name,string src){
    DIR *dirref1;
    bool isThere = false;
    dirref1 = opendir(src.c_str());
    struct dirent *eleref;
    eleref = readdir(dirref1);
    while(eleref != NULL){
        string cname = eleref->d_name;
        if(cname == name){
            isThere = true;
            break;
        }
        eleref = readdir(dirref1);
    }
    closedir(dirref1);
    return isThere;
}
// Main

int main(void)
{

    // First time Value assignments
    len = 0;
    cursor = 0;
    fstart = 0;
     currPath = "/home/";
    elementsOfPrint = new string *[500];
    firstTimeNM = true;
    want_to_exit = false;

    while (!want_to_exit)
    {
        goToNormalMode();
        if (!want_to_exit)
            goToCommandMode();
    }

    printf("\33c");
    // cout<<existsInSrc("main.cpp","/home/shukan/Documents/AOS/Assignment1");
    // CreateNewFolder("HarikrishnaMaharaj","/home/shukan/Music");
    // copyDirectory("/home/shukan/Documents/AOS/Sample3","Q3","/home/shukan/Music/Q3");
    // cout<<calculateAbsPath("Documents/.//.SSD/");
    return 0;
}
