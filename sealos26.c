//COPYLEFT FROM ZILELAI/ZL PROJECTS 2026
//THIS FILE IS LICENSED BY GNU 3.0 LICENSE IN GITHUB





#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <float.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef _WIN32
    #include <direct.h>
    #include <io.h>
    #define mkdir(dir, mode) _mkdir(dir)
    #define rmdir(dir) _rmdir(dir)
#else
    #include <sys/stat.h>
    #include <unistd.h>
    #include <dirent.h>
#endif

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;

    char *ptr = (char *)realloc(*(char **)userp, strlen(*(char **)userp) + realsize + 1);
    if(ptr == NULL) return 0; 
    
    *(char **)userp = ptr;
    
    strncat(*(char **)userp, (const char *)contents, realsize);
    return realsize;
}

char op;
double firstnum;
double secondnum;
double result;
char cmd[50];
char input_buf[50];
int loc = 0;
int notes_mode = 0;
char notes_name[64] = "notes.txt";
char notes_path[128];
int q = 1;
char w = 'A';
float e = 5.7293;
double r = 4.882823923923923823283;
long t = 832832832732;




void get_current_path(int current_loc, const char* filename, char* out_path) {
    switch(current_loc) {
        case 1: sprintf(out_path, "home/%s", filename); break;
        case 2: sprintf(out_path, "documents/%s", filename); break;
        case 3: sprintf(out_path, "downloads/%s", filename); break;
        case 4: sprintf(out_path, "system/%s", filename); break;
        default: sprintf(out_path, "%s", filename); break; 
    }
}

void list_documents_contents() {
#ifdef _WIN32
    struct _finddata_t c_file;
    intptr_t hFile;

    if ((hFile = _findfirst("documents/*", &c_file)) != -1L) {
        do {
            if (strcmp(c_file.name, ".") != 0 && strcmp(c_file.name, "..") != 0) {
                if (c_file.attrib & _A_SUBDIR) {
                    printf("   - documents/%s/\n", c_file.name);
                } else {
                    printf("   - documents/%s\n", c_file.name);
                }
            }
        } while (_findnext(hFile, &c_file) == 0);
        _findclose(hFile);
    }
#else
    DIR *d = opendir("documents");
    struct dirent *dir;
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                if (dir->d_type == DT_DIR) {
                    printf("  └─ documents/%s/\n", dir->d_name);
                } else {
                    printf("  └─ documents/%s\n", dir->d_name);
                }
            }
        }
        closedir(d);
    }
#endif
}

int game(char player, char bot){
    if (player == bot){
        return -1;
    }

    if (player == 's' && bot == 'p'){
        return 0;
    }

    else if (player == 'p' && bot == 's'){
        return 1;
    }

    else if (player == 's' && bot == 'r'){
        return 1;
    }
    else if (player == 'r' && bot == 's'){
        return 0;
    }
    else if (player == 'r' && bot == 'p'){
        return 1;
    }
    else if (player == 'p' && bot == 'r'){
        return 0;
    }
}



void process_system_command(char *input) {
    time_t currentTime;
    char content[256];
    FILE *file;

    input[strcspn(input, "\n")] = 0;
    if (strlen(input) == 0) return;

    char cmd[20] = "";
    char arg1[64] = "";
    char arg2[64] = "";
    int parsed_args = sscanf(input, "%s %s %s", cmd, arg1, arg2);

    if (strcmp(cmd, "chmod") == 0) {
        if (parsed_args < 3) {
            printf("err: 0, 1 only found\n");
        } else if (strcmp(arg1, notes_name) != 0) {
            printf("err: file missing\n");
        } else {
            notes_mode = atoi(arg2);
            printf("changed\n");
        }
    }
    else if (strcmp(cmd, "rm") == 0) {
        if (parsed_args < 2) {
            printf("err: missing filename\n");
        } else {
            char target_path[128];
            get_current_path(loc, arg1, target_path);

            if (remove(target_path) == 0) {
                if (strcmp(notes_name, arg1) == 0) {
                    strcpy(notes_name, "notes.txt");
                    notes_mode = 0;
                }
                printf("changed\n");
            } else {
                printf("err: file not found\n");
            }
        }
    }
    else if (strcmp(cmd, "save") == 0) {
        if (parsed_args < 2) {
            printf("err: missing URL or Script file\n");
        } else {
            if (strstr(arg1, ".seal") != NULL) {
                printf("Script %s configuration saved successfully.\n", arg1);
            } else {
                printf("Connecting to remote server...\n");
                printf("Downloading resources from: %s\n", arg1);
                
                if (strstr(arg1, "codepad.app/pad/822052z5n") != NULL) {
                    FILE *dl = fopen("downloads/flag.txt", "w");
                    if (dl) {
                        fprintf(dl, "SEAL{c_strings_are_pointers_to_fun}\n");
                        fclose(dl);
                        printf("Saved successfully to downloads/flag.txt\n");
                    }
                } else {
                    FILE *dl = fopen("downloads/index.html", "w");
                    if (dl) {
                        fprintf(dl, "\n<h1>SealOS Sandbox Landing</h1>\n");
                        fclose(dl);
                        printf("Saved successfully to downloads/index.html\n");
                    }
                }
            }
        }
    }
    else if (strcmp(cmd, "exe") == 0) {
        if (parsed_args < 2) {
            printf("err: missing script file\n");
        } else {
            char script_path[128];
            get_current_path(loc, arg1, script_path);
            
            FILE *script = fopen(script_path, "r");
            if (!script) {
                printf("err: script file not found\n");
            } else {
                char script_line[100];
                printf("---  %s ---\n", arg1);
                while (fgets(script_line, sizeof(script_line), script) != NULL) {
                    script_line[strcspn(script_line, "\r\n")] = 0;
                    if (strlen(script_line) == 0) continue; 
                    
                    printf("[%s] executing: %s\n", arg1, script_line);
                    process_system_command(script_line); 
                }
                fclose(script);
            }
        }
    }
    else if (strcmp(cmd, "rnm") == 0) { 
        if (parsed_args < 2) {
            printf("err: missing filename\n");
        } else {
            char old_path[128], new_path[128], new_name[64];
            sprintf(old_path, "documents/%s", arg1);
            
            FILE *check = fopen(old_path, "r");
            if (!check) {
                printf("err: file missing\n");
            } else {
                fclose(check);
                printf("rename: ");
                fflush(stdout);
                if (fgets(new_name, sizeof(new_name), stdin) != NULL) {
                    new_name[strcspn(new_name, "\n")] = 0;
                    sprintf(new_path, "documents/%s", new_name);

                    if (rename(old_path, new_path) == 0) {
                        if (strcmp(notes_name, arg1) == 0) {
                            strcpy(notes_name, new_name);
                        }
                        printf("changed\n");
                    } else {
                        printf("err: rename unfound\n");
                    }
                }
            }
        }
    }
    else if (strcmp(cmd, "mkfile") == 0) { 
        if (parsed_args < 2) {
            printf("err: missing name\n");
        } else {
            char folder_path[128];
            sprintf(folder_path, "documents/%s", arg1);
            
            if (mkdir(folder_path, 0777) == 0) {
                printf("changed\n");
            } else {
                printf("err: folder unable to generate\n");
            }
        }
    }
    else if (strcmp(cmd, "ls-d") == 0) {
        printf("prwxf---o---surwx, system/sys.txt\n");
#ifdef _WIN32
        struct _finddata_t c_file;
        intptr_t hFile;
        if ((hFile = _findfirst("documents/*", &c_file)) != -1L) {
            do {
                if (strcmp(c_file.name, ".") != 0 && strcmp(c_file.name, "..") != 0) {
                    if (c_file.attrib & _A_SUBDIR) {
                        printf("drwxf---o---surwx, documents/%s/\n", c_file.name);
                    } else {
                        if (notes_mode == 1) printf("prwxf---o---surwx, documents/%s\n", c_file.name);
                        else printf("p---f---o---surwx, documents/%s\n", c_file.name);
                    }
                }
            } while (_findnext(hFile, &c_file) == 0);
            _findclose(hFile);
        }
#else
        DIR *d = opendir("documents");
        struct dirent *dir;
        if (d) {
            while ((dir = readdir(d)) != NULL) {
                if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                    if (dir->d_type == DT_DIR) {
                        printf("drwxf---o---surwx, documents/%s/\n", dir->d_name);
                    } else {
                        if (notes_mode == 1) printf("prwxf---o---surwx, documents/%s\n", dir->d_name);
                        else printf("p---f---o---surwx, documents/%s\n", dir->d_name);
                    }
                }
            }
            closedir(d);
        }
#endif
    }
    else if (strcmp(cmd, "w") == 0) { 
        if (parsed_args >= 2) {
            strcpy(notes_name, arg1);
        }
        if (notes_mode == 0 && parsed_args >= 2 && strcmp(arg1, "notes.txt") == 0) {
            printf("err: permission denied \n");
        } else {
            get_current_path(loc, notes_name, notes_path);
            file = fopen(notes_path, "w");
            if (file) {
                if (strstr(notes_name, ".seal") != NULL) {
                    printf("ZL SCRIPT EDITOR (%s)\n ", notes_name);
                } else {
                    printf("ZL FILE EDITOR (%s)\nEnter text: ", notes_name);
                }
                fflush(stdout);
                fgets(content, sizeof(content), stdin);
                fprintf(file, "%s", content);
                fclose(file);
                printf("Saved successfully.\n");
            } else {
                printf("err: could not create file\n");
            }
        }
    }
    else if (strcmp(cmd, "pad") == 0) { 
        if (parsed_args >= 2) {
            strcpy(notes_name, arg1);
        }
        
        if (notes_mode == 0 && parsed_args >= 2 && strcmp(arg1, "code.txt") == 0) {
            printf("err: permission denied \n");
        } else {
            printf("PAD EDITOR (%s)\n", notes_name);
            printf("Type exit to save and exit\n\n");
            char line_buffer[512]; 
            get_current_path(loc, notes_name, notes_path);
            file = fopen(notes_path, "a+");
            
            if (file) {
                fseek(file, 0, SEEK_SET); 
                while (fgets(line_buffer, sizeof(line_buffer), file) != NULL) {
                    printf("%s", line_buffer);
                }
                while (1) {
                    fflush(stdout);
                    if (fgets(content, sizeof(content), stdin) == NULL) {
                        break; 
                    }
                    if (strcmp(content, "exit\n") == 0) {
                        break; 
                    }
                    fprintf(file, "%s", content);
                }
                fclose(file);
                printf("saved\n");
            } else {
                printf("err: could not open file\n");
            }
        }
    }
    else if (strcmp(cmd, "r") == 0) {
        if (parsed_args < 2) {
            printf("err: missing filename\n");
        } else {
            if (strcmp(arg1, notes_name) == 0 && notes_mode == 0) {
                printf("err: permission denied\n");
            } else {
                char read_path[128], line_buffer[256];
                get_current_path(loc, arg1, read_path);
                FILE *rf = fopen(read_path, "r");
                if (rf) {
                    printf("\n---------------------\n"); 
                    while (fgets(line_buffer, sizeof(line_buffer), rf) != NULL) {
                        printf("%s", line_buffer);
                    }
                    printf("\n---------------------\n"); 
                    fclose(rf);
                } else {
                    printf("err: file locked or missing\n");
                }
            }
        }
    }
    else if (strcmp(cmd, "info") == 0) {

        printf("                 ..^~:::::::::::....              \n");
            printf("            .::::.......:^::.. ..:..:::.          \n");
            printf("       ...::..          .:.:.  .::.   .:^:        \n");
            printf("      ::..              .: ~:. ::..      .::      \n");
            printf("    ::.   ..^ :^..      .. . :. ...  :     :^.    \n");
            printf("   .^    P@G:  7&#~      ...         ~       ^.   \n");
            printf("  :^     ~!::!^.:!^  .   ::. ::::.^  ~. .    .^   \n");
            printf("  ^:     ~^?B@@B!:~  ^     .:^..:::...:..     !^. \n");
            printf(" ^:^    ^77GP5PPJ!^  ........ ...  .:   . ^: :^ ~ \n");
            printf(" ^.^:   :...    ^^         ....... :^:^^~:: .^.:^ \n");
            printf("  ..:::...:^:.  ^. .. .. .^.......^^..^:...:^:..  \n");
            printf("       ....:::::^^^~~^^^:^~ : ^:::^^.......       \n");
            printf("                   .......^:~^~~^^.               \n");
            printf("\n");
        printf("SealKernel 25 \n");
        printf("Code Env.: CodePad\n");
        printf("PC Info: Virtual\n");
        printf("Copyleft SealKernel from ZL Project\n"); 
        printf("2026\n");
        
        }
        
        
        else if (strcmp(cmd, "about") == 0) {
            printf("                 ..^~:::::::::::....              \n");
            printf("            .::::.......:^::.. ..:..:::.          \n");
            printf("       ...::..          .:.:.  .::.   .:^:        \n");
            printf("      ::..              .: ~:. ::..      .::      \n");
            printf("    ::.   ..^ :^..      .. . :. ...  :     :^.    \n");
            printf("   .^    P@G:  7&#~      ...         ~       ^.   \n");
            printf("  :^     ~!::!^.:!^  .   ::. ::::.^  ~. .    .^   \n");
            printf("  ^:     ~^?B@@B!:~  ^     .:^..:::...:..     !^. \n");
            printf(" ^:^    ^77GP5PPJ!^  ........ ...  .:   . ^: :^ ~ \n");
            printf(" ^.^:   :...    ^^         ....... :^:^^~:: .^.:^ \n");
            printf("  ..:::...:^:.  ^. .. .. .^.......^^..^:...:^:..  \n");
            printf("       ....:::::^^^~~^^^:^~ : ^:::^^.......       \n");
            printf("                   .......^:~^~~^^.               \n");
            printf("\n");
        
            FILE *fp = fopen("main.cpp", "rb");

        if (fp == NULL){
            perror("err: file unopened");
            
        }

        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        

        fclose(fp);

        printf("SealKernel 25 \n");
        
        printf("Code Env.: VM\n");
        
        printf("Code Env. 2: CodePad\n");
        
        printf("Code: C, C++\n");
        
        printf("Host: CodePad Server\n");
        
        printf("PC Info: Virtual\n");

        printf("SealKernel Size: %ld bytes\n", size);
        
        printf("Copyleft SealKernel from ZL Project\n");
        
        printf("2026\n");
        
        printf("QWERTYUIOPASDFGHJKLZXCVBNM1234567890\n");
        
        }
        
        
            
    else if (strcmp(cmd, "sizeofint") == 0){
        printf("%zu\n", sizeof(q));
    }
    else if (strcmp(cmd, "sizeofchar") == 0){
        printf("%zu\n", sizeof(w));
    }
    else if (strcmp(cmd, "sizeoffloat") == 0){
        printf("%zu\n", sizeof(e));
    }
    else if (strcmp(cmd, "sizeoflong") == 0){
        printf("%zu\n", sizeof(t));
    }
    else if (strcmp(cmd, "sizeofdouble") == 0){
        printf("%zu\n", sizeof(r));
    }
    else if (strcmp(cmd, "goto") == 0) {
        if (strcmp(arg1, "home") == 0) loc = 1;
        else if (strcmp(arg1, "documents") == 0) loc = 2;
        else if (strcmp(arg1, "downloads") == 0) loc = 3;
        else if (strcmp(arg1, "system") == 0) loc = 4;
        else printf("err: unknown folder\n");
    }

    else if (strcmp(cmd, "sudo-on") == 0){
        loc = 5;
        printf("sudo mode\n");
    }
    else if (strcmp(cmd, "sudo-off") == 0){
        loc = 0;
        printf("normal mode\n");
    }
    else if (strcmp(cmd, "back") == 0) {
        loc = 0;
    }
    else if (strcmp(cmd, "where") == 0) {
        const char* locations[] = {"root", "home", "documents", "downloads", "system", "superior"};
        if (loc >= 0 && loc <= 5) printf("%s\n", locations[loc]);
    }
    else if (strcmp(cmd, "version") == 0) {
        printf("SealKernel 24\n");
    }
    else if (strcmp(cmd, "release") == 0){
        printf("SealKernel 10 - can check size of variable class and can check version and release.\n");printf("SealKernel 11 - added curl to grab data from one site and added fast OS specification.\n");printf("SealKernel 12 [BETA] - added tsastream command to check streaming marks for TSIS student\n");printf("SealKernel 13 - Added calculator function andd improved tsastream\n"); printf("SealKernel 14 [BETA] - added tic tac toe game\n");printf("SealKernel 15 - added check storage in main.cpp\n");printf("SealKernel 16 - added check storage in main.cpp inside quick and about and removed TIC TAC TOE for ROCK PAPER SCISSORS game\n");printf("SealKernel 17 - fixed rock paper scissors game and added guess the number game\n"); printf ("SealKernel 18 - changed file locations and changed space-main.cpp to space. Also fixed game1\n");printf("SealKernel 19 - added game3 and game4 and also restricted exit command only for sudo user\n");printf("SealKernel 20 - added dice feature\n");printf("SealKernel 21 - fixed tsastream and removed quick and about for monthly cleaning (July\n");    printf("SealKernel 22 - Improved tsastream");printf("SealKernel 23 - added luck program, element program and game5. Also improved pad function");printf("SealKernel 24 - added speed reaction game\n");printf("SealKernel25 - Added conquer country game and also added bool. Also addded more space function (check out in help)");}
    else if (strcmp(cmd, "ls") == 0) {
        printf("home\ndownloads\ndocuments\nsystem\n");
        FILE *sys_check = fopen("system/sys.txt", "r");
        if (sys_check) {
            printf("system/sys.txt\n");
            fclose(sys_check);
        }
        
        if (loc == 3) { 
            FILE *f_html = fopen("downloads/index.html", "r");
            if (f_html) { printf("downloads/index.html\n"); fclose(f_html); }
            FILE *f_flag = fopen("downloads/flag.txt", "r");
            if (f_flag) { printf("downloads/flag.txt\n"); fclose(f_flag); }
        } else {
            list_documents_contents();
        }
    }
    else if (strcmp(cmd, "help") == 0) {
        printf("SEALOS SCRIPT LIST:\n");
        printf("goto (Folder) - goes to 1 folder\n");
        printf("ls - list folders and files out\n");
        printf("ls-d - list folders and files out detailed\n");
        printf("info - show OS specification\n");
        printf("about - show OS specification\n");
        printf("w notes.txt - create/write a file called notes.txt\n");
        printf("r notes.txt - read the contents of notes.txt\n"); 
        printf("rnm notes.txt - rename notes.txt to something else\n");
        printf("chmod notes.txt 1 - change notes.txt to public\n");
        printf("chmod notes.txt 0 - change notes.txt to private\n");
        printf("mkfile (folder name) - create (folder)\n"); 
        printf("where - to show where you are\n");
        printf("back - go back to root\n");
        printf("eggs - secret\n");
        printf("lemon - secret\n");
        printf("echo (text) - repeat (text)\n");
        printf("date - show current date and time\n");
        printf("random - show random numbers\n");
        printf("rm - remove file\n");
        printf("save - download files from websites\n");
        printf("w [name].seal - create a executable script\n");
        printf("exe [name].seal - run .seal script\n");
        printf("sizeof(variable class) - check size of variable class\n");
        printf("release - check what has updated\n");
        printf("version - check only the version\n");
        printf("curl - search something online (might not work in online compiler)\n");
        printf("tsastream - check your avg score for TSIS students\n");
        printf("calc - simple calculator that you have to key in manually\n");
        printf("space - to check space in main.cpp\n");
        printf("space-downloads - to check space in downloads\n");
        printf("space-documents - to check space in documents\n");
        printf("space-home - to check space in home\n");
        printf("space-system - to check space in system\n");
        printf("space-others - to check space in other file/directory(folder)\n");
        printf("game1 - plays rock paper scissors game (r for rock, s for scissors and p for paper)\n");
        printf("game2 - guess the number game (type the number from 0 to 99 \n)");
        printf("sudo-on - change to sudo user\n");
        printf("sudo-off - change to normal user \n");
        printf("game3 - flag capture game\n");
        printf("game4 - gun game\n");
        printf("sudo-exit - shut down only with sudo permissions\n");
        printf("dice - roll a dice\n");
        printf("luck - program that determines (will or wont) your luck\n");
        printf("game5 - let the bot guess your number (from 0 to 99)\n");
        printf("elements - let the system choose an element\n");
        printf("game6 - Reaction Time Test\n");
        printf("game7 - conquer country games\n");
        printf("bool - system-controlled true-false answer\n");
    }
    else if (strcmp(cmd, "echo") == 0) {
        printf("%s\n", input + (strlen(input) > 4 ? 5 : 0));
    }
    else if (strcmp(cmd, "tsastream") == 0) {
        
        
        float math;
        float chemistry;
        float physics;
        float biology;
        float history;
        float geography;
        
        
        printf("Input your math marks: ");
        scanf("%f", &math);
        printf("Input your chemistry marks: ");
        scanf("%f", &chemistry);
        printf("Input your biology marks: ");
        scanf("%f", &biology);
        printf("Input your physics marks: ");
        scanf("%f", &physics);
        printf("Input your geography marks: ");
        scanf("%f", &geography);
        printf("Input your history marks: ");
        scanf("%f", &history);


        float math_pct = math * 0.55;
        float chem_pct = chemistry * 0.125;
        float bio_pct = biology * 0.1;
        float phy_pct = physics * 0.125;
        float geo_pct = geography * 0.05;
        float his_pct = history * 0.05;

        int avg = math_pct+chem_pct+bio_pct+phy_pct+geo_pct+his_pct;

        
        printf("Your streaming mark is: %.d%%\n", avg);
        if (avg >= 90){
            printf("2025 streaming mark: Acacia\n");
        }
        else if (avg >= 80 && avg < 90){
            printf("2025 streaming mark: Aster\n");
        }

        else if (avg >= 70 && avg < 80){
            printf("2025 streaming mark: Begonia\n");
        }

        else if (avg >= 60 && avg < 70){
            printf("2025 streaming mark: Castanea\n");
        }

        else if (avg >= 50 && avg < 60){
            printf("2025 streaming mark: Juniper / MX Intensive 1\n");
        }

        else if (avg < 50){
            printf("2025 streaming mark: Magnolia/ MX Intensive 2 / MX Intensive\n");
        }

        if (avg >= 70){
            printf("2026 streaming mark: Acacia\n");
        }
        else if (avg >= 60 && avg < 70){
            printf("2026 streaming mark: Aster\n");
        }

        else if (avg >= 50 && avg < 60){
            printf("2026 streaming mark: Begonia\n");
        }

        else if (avg >= 40 && avg < 50){
            printf("2026 streaming mark: Castanea\n");
        }

        else if (avg < 40){
            printf("2026 streaming mark: MX Intensive 1 & 2\n");
        }
        


        
    }
    else if (strcmp(cmd, "date") == 0) {
        time(&currentTime); 
        printf("%s", ctime(&currentTime));
    }
    
    else if (strcmp(cmd, "random") == 0) {
        printf("%d\n", rand());
    }
    else if (strcmp(cmd, "lemon") == 0) {
        printf("..................................................\n"
               "...........:::^^~~~~~~~~~^^::.....................\n"
               ".......::^~~~!!!!!!!!!!777777!~^::................\n"
               "...::^~~~~~~~~~~~~!!!!!7777777??77!~:.............\n"
               "..:~~~~~~~~~~~~~~~!!!!7777777777?????!^:..........\n"
               ".:^~~~~~~~~~~~~~!!!!!!!777777777?????J??~:........\n"
               ".:~~~~~~~~~~~~!~!!!!!77777777777???????JJ7^.......\n"
               "..^~~~~~~~!!!!!!!!7777777777?????????????7~......\n"
               "..:~~~~!!!!!!!77777???77???????????????????7^.....\n"
               "..:^~~!!!!!!777777??????????????????????????~.....\n"
               "...:~~~!!!!!7777?7????????JJJ?JJJJ????????JJ?^....\n"
               "....^~~~!!!!77777777???????JJJJJ?????????J????7:..\n"
               ".....:~~!!!!!777777777?????????????????????JJJJ^..\n"
               "......:^~~!!!!!!777777777??????????????????7!!^...\n"
               "........::^~!!!!!7777777777??????????????!:.......\n"
               "...........::^~!!!!777777777???????????!:.........\n");
    }
    else if (strcmp(cmd, "eggs") == 0) {
        printf("                              \n"
               "            .::.              \n"
               "           :!~~~~  ...        \n"
               "          :77!~~7~~!~~~:      \n"
               "        ..^7YJ?JJY7!~!7!      \n"
               "       ^~~^^7JJ?7YYJJJ7^      \n"
               "     .:7?7!~~!~:^!?J?!^.      \n"
               "    ..^!?YYJJ7^..::^:..       \n"
               "     .:^~!77!^:...            \n");
    }
    else if (strcmp(cmd, "calc") == 0) {
        printf("Enter operator:\n");
        scanf(" %c\n", &op);
        printf("Enter first number:\n");
        scanf(" %lf\n", &firstnum);
        printf("Enter second number:\n");
        scanf(" %lf\n", &secondnum);

        switch (op) {
            case '+':
                result = firstnum + secondnum;
                break;
            case '-':
                result = firstnum - secondnum;
                break;
            case 'x':
                result = firstnum * secondnum;
            case '/' :
                result = firstnum / secondnum;
            case '*':
                result = firstnum * secondnum;
            default:

                printf("err: operator unfound\n");
                result = -DBL_MAX;
        }
        if (result!=-DBL_MAX){
            printf("%.2lf\n", result);
        }
            
    }
    

    else if (strcmp(cmd, "space") == 0){
        FILE *fp = fopen("main.cpp", "rb");
        

        if (fp == NULL){
            perror("err: file unopened");
            
        }

        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        printf("main.cpp: %ld bytes\n", size);

        fclose(fp);
    }

    else if (strcmp(cmd, "space-documents") == 0){
        FILE *fp = fopen("documents", "rb");
        

        if (fp == NULL){
            perror("err: file unopened");
            
        }

        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        printf("documents: %ld bytes\n", size);

        fclose(fp);
    }

    else if (strcmp(cmd, "space-downloads") == 0){
        FILE *fp = fopen("downloads", "rb");
        

        if (fp == NULL){
            perror("err: file unopened");
            
        }

        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        printf("downloads: %ld bytes\n", size);

        fclose(fp);
    }

    else if (strcmp(cmd, "space-home") == 0){
        FILE *fp = fopen("home", "rb");
        

        if (fp == NULL){
            perror("err: file unopened");
            
        }

        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        printf("home: %ld bytes\n", size);

        fclose(fp);
    }
    else if (strcmp(cmd, "space-system") == 0){
        FILE *fp = fopen("system", "rb");
        

        if (fp == NULL){
            perror("err: file unopened");
            
        }

        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        printf("system: %ld bytes\n", size);

        fclose(fp);
    }

    else if (strcmp(cmd, "space-others") == 0){
        char input [256];
        printf("What file or folder(directory)? ");
        scanf("%255s", input);
        FILE *fp = fopen(input, "rb");
        

        if (fp == NULL){
            perror("err: file unopened");
            return;
            
        }

        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        printf("%s", input);
        printf(": %ld bytes\n", size);

        fclose(fp);
    }

    else if (strcmp(cmd, "game1") == 0){
        int n = rand() % 3;
        char player, bot, result;
        srand(time(NULL));
        
        if(n == 0){
            bot = 's';
        }
        else if (n == 1){
            bot = 'p';
        }

        else{
            bot = 'r';
        }

        

        scanf("%c", &player);

        result = game(player, bot);

        if (result == -1) {
            printf("YOU TIED! WUNDERBAR!\n");
        }
        else if (result == 0) {
            printf("YOU WIN!\n");
        }
        else { 
            printf("YOU LOST!\n");
        }
            printf("You choose : %c and Bot choose : %c\n",player, bot);

    
    }

    else if (strcmp(cmd, "game2") == 0){
        int n = rand() % 50;
        srand(time(NULL));
        int player;
        int bot;
        scanf("%i", &player);
        if (player == bot){
            printf("You win! You guessed it\n");
        }
        else {
            printf("Guess incorrect, guess again\n");
        }
    }

    else if (strcmp(cmd, "game3") == 0){
        int input;
        printf("Flag Capture\n");
        printf("YOUR LOADOUT: 1. Healing Staff, 2. Sniper, 3. BHG, 4. Grenade Launcher");
        printf("1 to 4 to change loadout\n");
        printf("goal: grab the flag and put in your base\n");
        printf("someone is attacking you\n");
        printf("choose your weapon\n");
        printf("he is having a sniper far away\n");

        scanf("%d", &input);
        if (input == 1){
            printf("You die because of headshot\n");
        }
        else if (input == 2){
            printf("Killed opponent, attacked 100 dmg, faced 50 dmg\n");

            printf("GO AHEAD\n");
            sleep(100);
            printf ("someone is attacking you\n");
            printf ("he is having a shotgun\n");
            printf("he is near\n");
            scanf("%d", &input);
            if (input == 1){
                printf ("Killed opponent. Attacked 100dmg, dealt 90 damage.\n" );
                printf("GO AHEAD\n");
                sleep(100);
                printf("someone is attacking you\n");
                printf ("he is having a rpg\n");
                printf("he is far away\n");
                scanf("%d", &input);

                if (input == 1){
                    printf("You die because of spam\n");
                }
                else if (input == 2){
                    printf("Killed opponent. Attacked 100dmg, dealt 90 dmg\n");
                    printf("GRABBED FLAG!!!\n");
                    sleep(100);
                    printf("someone is attacking you\n");
                    printf("he is having a BHG\n");
                    scanf("%d", &input);
                    if (input == 1){
                        printf("You die because of proness\n");
                    }
                    else if (input == 2){
                        printf("Killed opponent. Attacked 100dmg, dealt 70dmg\n");
                        printf("GOAL ACHIEVED\n");
                        printf("sucessfully put flag to your base\n");
                    }
                    else if (input == 3){
                        printf("You die because of proness\n");
                    }
                    else if (input == 4){
                        printf("You die because of proness\n");
                    }


                }
                else if (input == 3){
                    printf("Killed opponent. Attacked 100dmg, dealt 60dmg\n");
                    printf("GRABBED FLAG!!!\n");
                    sleep(100);
                    printf("someone is attacking you\n");
                    printf("he is having a BHG\n");
                    scanf("%d", &input);
                    if (input == 1){
                        printf("You die because of proness\n");
                    }
                    else if (input == 2){
                        printf("Killed opponent. Attacked 100dmg, dealt 70dmg\n");
                        printf("GOAL ACHIEVED\n");
                        printf("sucessfully put flag to your base\n");
                    }
                    else if (input == 3){
                        printf("You die because of proness\n");
                    }
                    else if (input == 4){
                        printf("You die because of proness\n");
                    }
                }
                else if (input == 4){
                    printf("You die because you killed yourself in the wall\n");
                }

            }
            else if (input == 2){
                printf("You die because of spam\n");
            }

            else if (input == 3){
                printf("Killed opponent. Attacked 100dmg, dealt 67 damage\n");
                printf("GO AHEAD\n");
                sleep(100);
                printf("someone is attacking you\n");
                printf ("he is having a rpg\n");
                printf("he is far away\n");
                scanf("%d", &input);

                if (input == 1){
                    printf("You die because of spam\n");
                }
                else if (input = 2){
                    printf("Killed opponent. Attacked 100dmg, dealt 90 dmg\n");
                    printf("GRABBED FLAG!!!\n");
                    sleep(100);
                    printf("someone is attacking you\n");
                    printf("he is having a BHG\n");
                    scanf("%d", &input);
                    if (input == 1){
                        printf("You die because of proness\n");
                    }
                    else if (input == 2){
                        printf("Killed opponent. Attacked 100dmg, dealt 70dmg\n");
                        printf("GOAL ACHIEVED\n");
                        printf("sucessfully put flag to your base\n");
                    }
                    else if (input == 3){
                        printf("You die because of proness\n");
                    }
                    else if (input == 4){
                        printf("You die because of proness\n");
                    }


                }
            }

            else if (input == 4){
                printf("You die because of spam \n");
            }

            
        }
        else if (input == 3){
            printf ("Killed opponent, attacked 100 dmg, faced 30 dmg\n");
            printf("GO AHEAD\n");
            sleep(10);
            printf ("someone is attacking you\n");
            printf ("he is having a shotgun\n");
            printf("he is near\n");
            scanf("%d", &input);
            if (input == 1){
                printf ("Killed opponent. Attacked 100dmg, dealt 90 damage.\n" );
                printf("GO AHEAD\n");
                sleep(10);
                printf("someone is attacking you\n");
                printf ("he is having a rpg\n");
                printf("he is far away\n");
                scanf("%d", &input);

                if (input == 1){
                    printf("You die because of spam\n");
                }
                else if (input == 2){
                    printf("Killed opponent. Attacked 100dmg, dealt 90 dmg\n");
                    printf("GRABBED FLAG!!!\n");
                    sleep(10);
                    printf("someone is attacking you\n");
                    printf("he is having a BHG\n");
                    scanf("%d", &input);
                    if (input == 1){
                        printf("You die because of proness\n");
                    }
                    else if (input == 2){
                        printf("Killed opponent. Attacked 100dmg, dealt 70dmg\n");
                        printf("GOAL ACHIEVED\n");
                        printf("sucessfully put flag to your base\n");
                    }
                    else if (input == 3){
                        printf("You die because of proness\n");
                    }
                    else if (input == 4){
                        printf("You die because of proness\n");
                    }


                }
                else if (input == 3){
                    printf("Killed opponent. Attacked 100dmg, dealt 60dmg\n");
                    printf("GRABBED FLAG!!!\n");
                    sleep(10);
                    printf("someone is attacking you\n");
                    printf("he is having a BHG\n");
                    scanf("%d", &input);
                    if (input == 1){
                        printf("You die because of proness\n");
                    }
                    else if (input == 2){
                        printf("Killed opponent. Attacked 100dmg, dealt 70dmg\n");
                        printf("GOAL ACHIEVED\n");
                        printf("sucessfully put flag to your base\n");
                    }
                    else if (input == 3){
                        printf("You die because of proness\n");
                    }
                    else if (input == 4){
                        printf("You die because of proness\n");
                    }
                }
                else if (input == 4){
                    printf("You die because you killed yourself in the wall\n");
                }

            }
        }

        else if (input == 4){
            printf("Killed opponent, attacked 140 dmg, faced 50 dmg\n");

            printf("GO AHEAD\n");
            sleep(100);
            printf ("someone is attacking you\n");
            printf ("he is having a shotgun\n");
            printf("he is near\n");
            scanf("%d", &input);
            if (input == 1){
                printf ("Killed opponent. Attacked 100dmg, dealt 90 damage.\n" );
                printf("GO AHEAD\n");
                sleep(10);
                printf("someone is attacking you\n");
                printf ("he is having a rpg\n");
                printf("he is far away\n");
                scanf("%d", &input);

                if (input == 1){
                    printf("You die because of spam\n");
                }
                else if (input == 2){
                    printf("Killed opponent. Attacked 100dmg, dealt 90 dmg\n");
                    printf("GRABBED FLAG!!!\n");
                    sleep(10);
                    printf("someone is attacking you\n");
                    printf("he is having a BHG\n");
                    scanf("%d", &input);
                    if (input == 1){
                        printf("You die because of proness\n");
                    }
                    else if (input == 2){
                        printf("Killed opponent. Attacked 100dmg, dealt 70dmg\n");
                        printf("GOAL ACHIEVED\n");
                        printf("sucessfully put flag to your base\n");
                    }
                    else if (input == 3){
                        printf("You die because of proness\n");
                    }
                    else if (input == 4){
                        printf("You die because of proness\n");
                    }


                }
                else if (input == 3){
                    printf("Killed opponent. Attacked 100dmg, dealt 60dmg\n");
                    printf("GRABBED FLAG!!!\n");
                    sleep(10);
                    printf("someone is attacking you\n");
                    printf("he is having a BHG\n");
                    scanf("%d", &input);
                    if (input == 1){
                        printf("You die because of proness\n");
                    }
                    else if (input == 2){
                        printf("Killed opponent. Attacked 100dmg, dealt 70dmg\n");
                        printf("GOAL ACHIEVED\n");
                        printf("sucessfully put flag to your base\n");
                    }
                    else if (input == 3){
                        printf("You die because of proness\n");
                    }
                    else if (input == 4){
                        printf("You die because of proness\n");
                    }
                }
                else if (input == 4){
                    printf("You die because you killed yourself in the wall\n");
                }

            }
        }
        
    }

    else if (strcmp(cmd, "game4") == 0) {
        int input;
        printf("Gun Game\n");
        printf("YOUR LOADOUT: 1. Knife, 2. Sniper, 3. Spamming Gun\n");
        printf("1 to 3 to change loadout\n");
        printf("ROUND START\n");
        printf("The person is near to you by 1m\n");
        printf("The person is using Rifle\n");
        printf("Choose your weapon\n");
        scanf("%d", &input);
        if (input == 1){
            printf("BACKSTAB! attacked: 100 damage, dealt 25 damage\n");
            sleep(5);
            printf("ROUND 2\n");
            printf("The person is near to you by 10m\n");
            printf("The person is using RPG\n");
            printf("Choose your weapon\n");
            scanf("%d", &input);
            if (input == 1){
                printf("You lost lol!!!\n");
            }
            else if (input == 2){
                printf("EASY HEADSHOT! attacked: 100 damage, dealt 25 damage\n");
                sleep(5);
                printf("MATCH POINT\n");
                printf("The person is near to you by 6m\n");
                printf("The person is using Uzi\n");
                printf("Choose your weapon\n");
                scanf("%d", &input);
                if (input == 1){
                    printf("You lost lol!!!\n");
                }
                else if (input == 2){
                    printf("You lost lol!!!\n");
                }
                else if (input == 3){
                    printf("EASY TAKEBACK! attacked: 100 damage, dealt 30 damage\n");
                    printf("YOU WIN!\n");
                }
            }
            else if (input == 3){
                printf("You lost lol!!!\n");
            }
        }
        else if (input == 2){
            printf("Killed! attacked: 100 damage, dealt 70 damage\n");
            
            sleep(5);
            printf("ROUND 2\n");
            printf("The person is near to you by 10m\n");
            printf("The person is using RPG\n");
            printf("Choose your weapon\n");
            scanf("%d", &input);
            if (input == 1){
                printf("You lost lol!!!\n");
            }
            else if (input == 2){
                printf("EASY HEADSHOT! attacked: 100 damage, dealt 25 damage\n");
                sleep(5);
                printf("MATCH POINT\n");
                printf("The person is near to you by 6m\n");
                printf("The person is using Uzi\n");
                printf("Choose your weapon\n");
                scanf("%d", &input);
                if (input == 1){
                    printf("You lost lol!!!\n");
                }
                else if (input == 2){
                    printf("You lost lol!!!\n");
                }
                else if (input == 3){
                    printf("EASY TAKEBACK! attacked: 100 damage, dealt 30 damage\n");
                    printf("YOU WIN!\n");
                }
            }
            else if (input == 3){
                printf("You lost lol!!!\n");
            }
        }
        else if (input == 3){
            printf("Killed! attacked: 100 damage, dealt 40 damage\n");
            sleep(5);
            printf("ROUND 2\n");
            printf("The person is near to you by 10m\n");
            printf("The person is using RPG\n");
            printf("Choose your weapon\n");
            scanf("%d", &input);
            if (input == 1){
                printf("You lost lol!!!\n");
            }
            else if (input == 2){
                printf("EASY HEADSHOT! attacked: 100 damage, dealt 25 damage\n");
                sleep(5);
                printf("MATCH POINT\n");
                printf("The person is near to you by 6m\n");
                printf("The person is using Uzi\n");
                printf("Choose your weapon\n");
                scanf("%d", &input);
                if (input == 1){
                    printf("You lost lol!!!\n");
                }
                else if (input == 2){
                    printf("You lost lol!!!\n");
                }
                else if (input == 3){
                    printf("EASY TAKEBACK! attacked: 100 damage, dealt 30 damage\n");
                    printf("YOU WIN!\n");
                }
            }
            else if (input == 3){
                printf("You lost lol!!!\n");
            }
        }
        
    }

    else if (strcmp(cmd, "dice") == 0){
        int n = rand() % 6;
        srand(time(NULL));
        printf ("%d\n", n);
    }

    else if (strcmp(cmd, "game6") == 0) {
        srand(time(NULL));
        int n = rand() % 9; 
        float delay_time = 1.0f + ((float)rand() / (float)RAND_MAX) * 2.0f;
        int input;
        usleep((useconds_t)(delay_time * 1000000));
        printf("Number: %d\n", n);
        scanf("%d", &input);
        if (input == n) {
            printf("YOU WIN!\n");
            printf("Your time reaction:%f\n", delay_time);
        } 
        else {
            printf("YOU LOSE!!\n");
        }
    }

    else if (strcmp(cmd, "luck") == 0){
        int n = rand() % 10;
        srand(time(NULL));
        if (n == 0){
            printf("NO LUCK\n");
        }
        else if (n == 1){
            printf("NO LUCK\n");
        }
        else if (n == 2){
            printf("YOU ARE LUCKY TODAY!!!!\n");
        }
        else if (n == 3){
            printf("Somebody told you to sac the queen\n");
        }
        else if (n == 4){
            printf("Somebody told you to sac the rook\n");
        }
        else if (n == 5){
            printf("If you don't know how to solve a cube, twist the corner and it would automatically solve the cube\n");
        }
        else if (n == 6){
            printf("YOU WONT GET 100 KILLS IN FLAG WARS (U SUCK BTW)\n");
        }
        else if (n == 7){
            printf("YOU WONT HAVE 4 WINSTREAK IN RIVALS TODAY\n");
        }
        else if (n == 8){
            printf("YOU WOULD GET 100 KILLS IN FLAG WARS\n");
        }
        else if (n == 9){
            printf("YOU WOULD HAVE 4 WINSTREAK IN RIVALS TODAY\n");
        }
        else if (n == 10){
            printf("I use arch btw...\n");
        }
    }

    else if (strcmp(cmd, "game5") == 0){
        int input;
        srand(time(NULL));
        int bot = rand() % 100;
        scanf("%d", &input);
        if (input == bot){
            printf("YOU LOSE. BOT's ANSWER IS SAME AS YOURS\n");
            printf("BOT:%d\n", bot);
            printf("BOT:%d\n", input);
            
        }
        else{
            printf("YOU WIN. BOT's ANSWER ISNT SAME AS YOURS\n");
            printf("BOT:%d\n", bot);
            printf("YOU:%d\n", input);
            
        }
    }

    else if (strcmp(cmd, "game7") == 0){
        int input;
        printf("You are a country\n");
        printf("You want to start a war\n");
        printf("There are other countries surrounding you\n");
        printf("Your strenght: 100\n");
        printf("Other countries: \n");
        printf("Country 1: 70 strenght, Country 2: 400 strenght, Country 3: 342 strenght, Country 4: 34 strenght, Country 5: 343 strenght, Country 6: 101 strenght\n");
        printf("Which one do you choose? ");
        scanf("%d", &input);
        if (input == 1){
            printf("YOU FIGHT WITH THEM!!! AND YOU GAINED TERRITORY\n");
            printf("YOUR STRENGHT: 120\n");
            printf("There are other countries surrounding you\n");
            printf("Your strenght: 100\n");
            printf("Other countries: \n");
            printf("Country 2: 400 strenght, Country 3: 342 strenght, Country 5: 343 strenght, Country 6: 101 strenght\n");
            printf("Which one do you choose? ");
            scanf("%d", &input);
            if (input == 2){
                printf("YOU FIGHT WITH THEM!!! AND YOU DIED AND GET CONQUERED\n");
            }
            else if (input == 3){
                printf("YOU FIGHT WITH THEM!!! AND YOU DIED AND GET CONQUERED\n");
            }
            else if (input == 5){
                printf("YOU FIGHT WITH THEM!!! AND YOU DIED AND GET CONQUERED\n");
            }
            else if (input == 6){
                printf("YOU FIGHT WITH THEM!!! AND YOU GAINED TERRITORY\n");
                printf("YOUR STRENGHT: 350\n");
                printf("There are other countries surrounding you\n");
  
                printf("Other countries: \n");
                printf("Country 2: 400 strenght, Country 3: 342 strenght, Country 5: 343 strenght\n");
                printf("Which one do you choose? ");
                scanf("%d", &input);
                if (input == 2){
                    printf("YOU FIGHT WITH THEM!!! AND YOU DIED AND GET CONQUERED\n");
                }
                else if (input == 3){
                    printf("YOU FIGHT WITH THEM!!! AND YOU GAINED TERRITORY\n");
                    printf("YOUR STRENGHT: 500\n");
                    printf("There is only one country surrounding you\n");
                    
                    printf("Other countries: \n");
                    printf("Country 2: 400 strenght\n");
                    printf("Which one do you choose? ");
                    scanf("%d", &input);
                }
                else if (input == 5){
                    printf("YOU FIGHT WITH THEM!!! AND YOU GAINED TERRITORY\n");
                    printf("YOUR STRENGHT: 499\n");
                    printf("There is only one country surrounding you\n");
                    
                    printf("Other countries: \n");
                    printf("Country 2: 400 strenght, Country 3: 342 strenght, Country 5: 343 strenght\n");
                    printf("Which one do you choose? ");
                    scanf("%d", &input);
                    if (input == 2){
                        printf("YOU FIGHT WITH THEM!!! AND YOU GAINED TERRITORY\n");
                        printf("YOU WIN!!\n");
                    }
                }
            }
        }
        else if (input == 2){
            printf("YOU FIGHT WITH THEM!!! AND YOU DIED AND GET CONQUERED\n");
        }
        else if (input == 3){
            printf("YOU FIGHT WITH THEM!!! AND YOU DIED AND GET CONQUERED\n");
        }
        else if (input == 4){
            printf("YOU FIGHT WITH THEM!!! AND YOU GAINED TERRITORY\n");
            printf("YOUR STRENGHT: 110\n");
            printf("There are other countries surrounding you\n");
     
            printf("Other countries: \n");
            printf("Country 2: 400 strenght, Country 3: 342 strenght, Country 5: 343 strenght, Country 6: 101 strenght\n");
            printf("Which one do you choose? ");
            scanf("%d", &input);
            if (input == 2){
                printf("YOU FIGHT WITH THEM!!! AND YOU DIED AND GET CONQUERED\n");
            }
            else if (input == 3){
                printf("YOU FIGHT WITH THEM!!! AND YOU DIED AND GET CONQUERED\n");
            }
            else if (input == 5){
                printf("YOU FIGHT WITH THEM!!! AND YOU DIED AND GET CONQUERED\n");
            }
            else if (input == 6){
                printf("YOU FIGHT WITH THEM!!! AND YOU GAINED TERRITORY\n");
                printf("YOUR STRENGHT: 350\n");
                printf("There are other countries surrounding you\n");
  
                printf("Other countries: \n");
                printf("Country 2: 400 strenght, Country 3: 342 strenght, Country 5: 343 strenght\n");
                printf("Which one do you choose? ");
                scanf("%d", &input);
                if (input == 2){
                    printf("YOU FIGHT WITH THEM!!! AND YOU DIED AND GET CONQUERED\n");
                }
                else if (input == 3){
                    printf("YOU FIGHT WITH THEM!!! AND YOU GAINED TERRITORY\n");
                    printf("YOUR STRENGHT: 500\n");
                    printf("There is only one country surrounding you\n");
                    
                    printf("Other countries: \n");
                    printf("Country 2: 400 strenght\n");
                    printf("Which one do you choose? ");
                    scanf("%d", &input);
                }
                else if (input == 5){
                    printf("YOU FIGHT WITH THEM!!! AND YOU GAINED TERRITORY\n");
                    printf("YOUR STRENGHT: 499\n");
                    printf("There is only one country surrounding you\n");
                    
                    printf("Other countries: \n");
                    printf("Country 2: 400 strenght, Country 3: 342 strenght, Country 5: 343 strenght\n");
                    printf("Which one do you choose? ");
                    scanf("%d", &input);
                    if (input == 2){
                        printf("YOU FIGHT WITH THEM!!! AND YOU GAINED TERRITORY\n");
                        printf("YOU WIN!!\n");
                    }
                }
            }
        }
        else if (input == 5){
            printf("YOU FIGHT WITH THEM!!! AND YOU DIED AND GET CONQUERED\n");
        }
        else if (input == 6){
            printf("YOU FIGHT WITH THEM!!! AND YOU DIED AND GET CONQUERED\n");
            
        }
        else {
            printf("Invalid input, so the guard kick you out\n");
        }

    }


    else if (strcmp(cmd, "elements") == 0){
        srand(time(NULL));
        int n = rand() % 119;
        if (n == 1){printf("Hydrogen");}
        else if (n == 2){printf("Helium");}
        else if (n == 3){printf("Lithium");}
        else if (n == 4){printf("Beryllium");}
        else if (n == 5){printf("Boron");}
        else if (n == 6){printf("Carbon");}
        else if (n == 7){printf("Nitrogen");}
        else if (n == 8){printf("Oxygen");}
        else if (n == 9){printf("Fluorine");}
        else if (n == 10){printf("Neon");}
        else if (n == 11){printf("Sodium");}
        else if (n == 12){printf("Magnesium");}
        else if (n == 13){printf("Aluminium");}
        else if (n == 14){printf("Silicon");}
        else if (n == 15){printf("Phosphorus");}
        else if (n == 16){printf("Sulfur");}
        else if (n == 17){printf("Chlorine");}
        else if (n == 18){printf("Argon");}
        else if (n == 19){printf("Potassium");}
        else if (n == 20){printf("Calcium");}
        else if (n == 21){printf("Scandium");}
        else if (n == 22){printf("Titanium");}
        else if (n == 23){printf("Vanadium");}
        else if (n == 24){printf("Chromium");}
        else if (n == 25){printf("Manganese");}
        else if (n == 26){printf("Iron");}
        else if (n == 27){printf("Cobalt");}
        else if (n == 28){printf("Nickel");}
        else if (n == 29){printf("Copper");}
        else if (n == 30){printf("Zinc");}
        else if (n == 31){printf("Gallium");}
        else if (n == 32){printf("Germanium");}
        else if (n == 33){printf("Arsenic");}
        else if (n == 34){printf("Selenium");}
        else if (n == 35){printf("Bromine");}
        else if (n == 36){printf("Krypton");}
        else if (n == 37){printf("Rubidium");}
        else if (n == 38){printf("Strontium");}
        else if (n == 39){printf("Yttrium");}
        else if (n == 40){printf("Zirconium");}
        else if (n == 41){printf("Niobidium");}
        else if (n == 42){printf("Molybdenum");}
        else if (n == 43){printf("Technetium");}
        else if (n == 44){printf("Rutbenium");}
        else if (n == 45){printf("Rhodium");}
        else if (n == 46){printf("Palladium");}
        else if (n == 47){printf("Silver");}
        else if (n == 48){printf("Cadmium");}
        else if (n == 49){printf("Indium");}
        else if (n == 50){printf("Tin");}
        else if (n == 51){printf("Antimony");}
        else if (n == 52){printf("Tellurium");}
        else if (n == 53){printf("Iodine");}
        else if (n == 54){printf("Xenon");}
        else if (n == 55){printf("Caesium");}
        else if (n == 56){printf("Barium");}
        else if (n == 57){printf("Lanthanium");}
        else if (n == 58){printf("Cerium");}
        else if (n == 59){printf("Praseodynium");}
        else if (n == 60){printf("Neodynium");}
        else if (n == 61){printf("Promethium");}
        else if (n == 62){printf("Samarium");}
        else if (n == 63){printf("Europium");}
        else if (n == 64){printf("Gadolinium");}
        else if (n == 65){printf("Terbium");}
        else if (n == 66){printf("Dysprosium");}
        else if (n == 67){printf("Holonium");}
        else if (n == 68){printf("Erbium");}
        else if (n == 69){printf("Thulium");}
        else if (n == 70){printf("Yttbium");}
        else if (n == 71){printf("Lutetium");}
        else if (n == 72){printf("Hafnium");}
        else if (n == 73){printf("Tantalum");}
        else if (n == 74){printf("Tungsten");}
        else if (n == 75){printf("Rhenium");}
        else if (n == 76){printf("Osmium");}
        else if (n == 77){printf("Iridium");}
        else if (n == 78){printf("Platinium");}
        else if (n == 79){printf("Gold");}
        else if (n == 80){printf("Mercury");}
        else if (n == 81){printf("Thallium");}
        else if (n == 82){printf("Lead");}
        else if (n == 83){printf("Bismuth");}
        else if (n == 84){printf("Polonium");}
        else if (n == 85){printf("Astatine");}
        else if (n == 86){printf("Radon");}
        else if (n == 87){printf("Francium");}
        else if (n == 88){printf("Radium");}
        else if (n == 89){printf("Actinium");}
        else if (n == 90){printf("Thorium");}
        else if (n == 91){printf("Protactinium");}
        else if (n == 92){printf("Uranium");}
        else if (n == 93){printf("Neptunium");}
        else if (n == 94){printf("Plutonium");}
        else if (n == 95){printf("Americium");}
        else if (n == 96){printf("Curinum");}
        else if (n == 97){printf("Berkelium");}
        else if (n == 98){printf("Californium");}
        else if (n == 99){printf("Einsteinium");}
        else if (n == 100){printf("Fermium");}
        else if (n == 101){printf("Mendelevium");}
        else if (n == 102){printf("Nobelium");}
        else if (n == 103){printf("Lawrencium");}
        else if (n == 104){printf("Rutherforium");}
        else if (n == 105){printf("Dubnium");}
        else if (n == 106){printf("Seaborgium");}
        else if (n == 107){printf("Seaborgium");}
        else if (n == 108){printf("Hassium");}
        else if (n == 109){printf("Meitnerium");}
        else if (n == 110){printf("Damstadtium");}
        else if (n == 111){printf("Roentgenium");}
        else if (n == 112){printf("Copemicium");}
        else if (n == 113){printf("Nihonium");}
        else if (n == 114){printf("Flerovium");}
        else if (n == 115){printf("Moscovium");}
        else if (n == 116){printf("Livermorium");}
        else if (n == 117){printf("Tenessine");}
        else if (n == 118){printf("Oganesson");}

        printf("\n");
    }

    else if (strcmp(cmd, "bool") == 0){
        srand(time(NULL));
        int n = rand() % 2;
        printf("%d\n", n);
    }

    else if (strcmp(cmd, "sudo-exit") == 0) {
        if (loc == 5){exit(0);}
        else {printf("err: sudo user undetected, switch to sudo user\n");}
        
    }

    else {
        printf("err: command not found\n");
    }

}


int main() {
    srand(time(NULL));
    char input[100];
    FILE *file;
    
    

    remove("system/sys.txt");
    remove("documents/notes.txt");
    rmdir("home"); rmdir("downloads"); rmdir("documents"); rmdir("system");

    mkdir("home", 0777);
    mkdir("downloads", 0777);
    mkdir("documents", 0777);
    mkdir("system", 0777);

    file = fopen("system/sys.txt", "w");
    if (file != NULL) {
        fprintf(file, "https://codepad.app/pad/822052z5n");
        fclose(file);
    }
    printf("SealKernel 25\n");
    printf("if don't know any command, use 'help'\n");

    while (1) {
        printf("seal> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) break;
        
        if (strcmp(input, "curl\n") == 0) {

            #ifdef LIBCURL_AVAILABLE
                printf("running curl...\n");
                break;
            #else
                printf("err: curl not supported here\n");
                continue;
            #endif
        }
        process_system_command(input);
    }


    #ifdef LIBCURL_AVAILABLE
        CURL *curl;
        CURLcode res;
        char *response_string = malloc(1); 
        if (response_string == NULL) return 1;
        response_string[0] = '\0';

        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init(); 

        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, "https://google.com");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

            res = curl_easy_perform(curl);

            if(res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            } else {
                printf("%s\n", response_string);
            }

            curl_easy_cleanup(curl);
        }
        
        free(response_string);
        curl_global_cleanup();
    #endif

    return 0;
}
