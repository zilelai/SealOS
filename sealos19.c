#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <float.h>
#include <sys/stat.h>

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
float math;
float chemistry;
float physics;
float biology;
float history;
float geography;

char board[] = {'-','-','-','-','-','-','-','-','-'};
int SENTINEL = -999;
int turncount = 0;



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
                    printf("  └─ documents/%s/\n", c_file.name);
                } else {
                    printf("  └─ documents/%s\n", c_file.name);
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
            get_current_path(loc, notes_name, notes_path);
            file = fopen(notes_path, "w");
            
            if (file) {
                if (strstr(notes_name, ".seal") != NULL) {
                    printf("PAD SCRIPT EDITOR (%s)\n", notes_name);
                    printf("Type exit to save and exit\n\n");
                } else {
                    printf("PAD FILE EDITOR (%s)\n", notes_name);
                    printf("Type exit to save and exit\n\n");
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
                printf("err: could not create file\n");
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
        printf("SealKernel 19 \n");
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

        printf("SealKernel 19 \n");
        
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
        
        else if (strcmp(cmd, "quick") == 0) {

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
            
            printf("SealKernel 19 \n");
            
            printf("Code Env.: CodePad\n");
            
            printf("Code: C, C++\n");
            
            printf("Host: CodePad Server\n");
            
            printf("PC Info: Virtual\n");

            printf("SealKernel Size: %ld bytes\n", size);
            
            printf("Copyleft SealKernel from ZL Project\n");
            
            printf("2026\n");
            
            printf("QWERTYUIOPASDFGHJKLZXCVBNM1234567890\n");
            
            } 

            else if (strcmp(cmd, "fast") == 0) {
                
                printf("-SealKernel/OS by ZL Project-🦭\n");
                printf("SealKernel 19 \n");
                printf("PC Info: Virtual\n");
                printf("Copyleft SealKernel from ZL Project\n");
                printf("2026\n");
                
                
                
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
        printf("SealKernel 19\n");
    }
    else if (strcmp(cmd, "release") == 0){
        printf("SealKernel 10 - can check size of variable class and can check version and release.\n");printf("SealKernel 11 - added curl to grab data from one site and added fast OS specification.\n");printf("SealKernel 12 [BETA] - added tsastream command to check streaming marks for TSIS student\n");printf("SealKernel 13 - Added calculator function andd improved tsastream\n"); printf("SealKernel 14 [BETA] - added tic tac toe game\n");printf("SealKernel 15 - added check storage in main.cpp\n");printf("SealKernel 16 - added check storage in main.cpp inside quick and about and removed TIC TAC TOE for ROCK PAPER SCISSORS game\n");printf("SealKernel 17 - fixed rock paper scissors game and added guess the number game\n"); printf ("SealKernel 18 - changed file locations and changed space-main.cpp to space. Also fixed game1\n");printf("SealKernel 19 - added game3 and game4 and also restricted exit command only for sudo user");
    }
    else if (strcmp(cmd, "ls") == 0) {
        printf("home/\ndownloads/\ndocuments/\nsystem/\n");
        FILE *sys_check = fopen("system/sys.txt", "r");
        if (sys_check) {
            printf("  - system/sys.txt\n");
            fclose(sys_check);
        }
        
        if (loc == 3) { 
            FILE *f_html = fopen("downloads/index.html", "r");
            if (f_html) { printf("  └─ downloads/index.html\n"); fclose(f_html); }
            FILE *f_flag = fopen("downloads/flag.txt", "r");
            if (f_flag) { printf("  └─ downloads/flag.txt\n"); fclose(f_flag); }
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
        printf("fast - show OS specification\n");
        printf("quick - show OS specification\n");
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
        printf("fast - show OS specification\n");
        printf("tsastream - check your avg score for TSIS students\n");
        printf("calc - simple calculator that you have to key in manually\n");
        printf("space - to check space in main.cpp\n");
        printf("game1 - plays rock paper scissors game (r for rock, s for scissors and p for paper)\n");
        printf("game2 - guess the number game (type the number from 1 to 100 \n)");
        printf("sudo-on - change to sudo user\n");
        printf("sudo-off - change to normal user \n");
        printf("game3 - flag capture game\n");
        printf("game4 - gun game");
        printf("sudo-exit - shut down only with sudo permissions");
    }
    else if (strcmp(cmd, "echo") == 0) {
        printf("%s\n", input + (strlen(input) > 4 ? 5 : 0));
    }
    else if (strcmp(cmd, "tsastream") == 0) {
        
        printf("First, type math marks, chemistry marks, physics marks, bio marks, geography marks and finally history marks before pressing enter\n");

        
        char input_buf[50];
        
        fgets(input_buf, sizeof(input_buf), stdin);
        for(int i=0; i<50; i++) { if(input_buf[i] == '\n') { input_buf[i] = '\0'; break; } }
        if (strcmp(input_buf, "") != 0) math = atof(input_buf);

        
        fgets(input_buf, sizeof(input_buf), stdin);
        for(int i=0; i<50; i++) { if(input_buf[i] == '\n') { input_buf[i] = '\0'; break; } }
        if (strcmp(input_buf, "") != 0) chemistry = atof(input_buf);

        
        fgets(input_buf, sizeof(input_buf), stdin);
        for(int i=0; i<50; i++) { if(input_buf[i] == '\n') { input_buf[i] = '\0'; break; } }
        if (strcmp(input_buf, "") != 0) physics = atof(input_buf);

        
        fgets(input_buf, sizeof(input_buf), stdin);
        for(int i=0; i<50; i++) { if(input_buf[i] == '\n') { input_buf[i] = '\0'; break; } }
        if (strcmp(input_buf, "") != 0) biology = atof(input_buf);

       
        fgets(input_buf, sizeof(input_buf), stdin);
        for(int i=0; i<50; i++) { if(input_buf[i] == '\n') { input_buf[i] = '\0'; break; } }
        if (strcmp(input_buf, "") != 0) history = atof(input_buf);

        
        fgets(input_buf, sizeof(input_buf), stdin);
        for(int i=0; i<50; i++) { if(input_buf[i] == '\n') { input_buf[i] = '\0'; break; } }
        if (strcmp(input_buf, "") != 0) geography = atof(input_buf);


        float math_pct = (math / 45.0) * 100.0;
        float chem_pct = (chemistry / 12.5) * 100.0;
        float bio_pct = (biology / 10.0) * 100.0;
        float phy_pct = (physics / 12.5) * 100.0;
        float geo_pct = (geography / 5.0) * 100.0;
        float his_pct = (history / 5.0) * 100.0;

        float avg = (math_pct + chem_pct + bio_pct + phy_pct + geo_pct + his_pct) / 6.0;
        printf("Your avg mark is: %.2f%%\n", avg);

        
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
            sleep(50);
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
                sleep(50);
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
            
            sleep(50);
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
                sleep(50);
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
            sleep(50);
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
                sleep(50);
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
    printf("SealKernel 19\n");
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
