#include <stdio.h>
#include <string.h>
#include <cstring>
#include <time.h>
#include <stdlib.h>


#include "raylib.h"

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


int loc = 0;
int notes_mode = 0;
char notes_name[64] = "notes.txt";
char notes_path[128];


#define MAX_LOG_LINES 26
#define MAX_LINE_LENGTH 128
char terminal_log[MAX_LOG_LINES][MAX_LINE_LENGTH];
int log_line_count = 0;

char current_input_buffer[64] = "";
int input_letter_count = 0;
int frame_cursor_counter = 0;


void raylib_printf(const char* format, ...) {
    char temp_buffer[256];
    va_list args;
    va_start(args, format);
    vsprintf(temp_buffer, format, args);
    va_end(args);


    char* line_token = strtok(temp_buffer, "\n");
    while (line_token != NULL) {
        if (log_line_count >= MAX_LOG_LINES) {
            // Shift older strings upward on full text viewports
            for (int i = 0; i < MAX_LOG_LINES - 1; i++) {
                strcpy(terminal_log[i], terminal_log[i + 1]);
            }
            strncpy(terminal_log[MAX_LOG_LINES - 1], line_token, MAX_LINE_LENGTH - 1);
            terminal_log[MAX_LOG_LINES - 1][MAX_LINE_LENGTH - 1] = '\0';
        } else {
            strncpy(terminal_log[log_line_count], line_token, MAX_LINE_LENGTH - 1);
            terminal_log[log_line_count][MAX_LINE_LENGTH - 1] = '\0';
            log_line_count++;
        }
        line_token = strtok(NULL, "\n");
    }
}

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
                    raylib_printf("  └─ documents/%s/\n", c_file.name);
                } else {
                    raylib_printf("  └─ documents/%s\n", c_file.name);
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
                    raylib_printf("  └─ documents/%s/\n", dir->d_name);
                } else {
                    raylib_printf("  └─ documents/%s\n", dir->d_name);
                }
            }
        }
        closedir(d);
    }
#endif
}


void process_system_command(char *input) {
    time_t currentTime;
    FILE *file;

    input[strcspn(input, "\n")] = 0;
    if (strlen(input) == 0) return;

    char cmd[20] = "";
    char arg1[64] = "";
    char arg2[64] = "";
    int parsed_args = sscanf(input, "%s %s %s", cmd, arg1, arg2);

    if (strcmp(cmd, "chmod") == 0) {
        if (parsed_args < 3) {
            raylib_printf("err: 0, 1 only found\n");
        } else if (strcmp(arg1, notes_name) != 0) {
            raylib_printf("err: file missing\n");
        } else {
            notes_mode = atoi(arg2);
            raylib_printf("changed\n");
        }
    }
    else if (strcmp(cmd, "rm") == 0) {
        if (parsed_args < 2) {
            raylib_printf("err: missing filename\n");
        } else {
            char target_path[128];
            get_current_path(loc, arg1, target_path);

            if (remove(target_path) == 0) {
                if (strcmp(notes_name, arg1) == 0) {
                    strcpy(notes_name, "notes.txt");
                    notes_mode = 0;
                }
                raylib_printf("changed\n");
            } else {
                raylib_printf("err: file not found\n");
            }
        }
    }
    else if (strcmp(cmd, "save") == 0) {
        if (parsed_args < 2) {
            raylib_printf("err: missing URL or Script file\n");
        } else {
            if (strstr(arg1, ".seal") != NULL) {
                raylib_printf("Script %s configuration saved successfully.\n", arg1);
            } else {
                raylib_printf("Connecting to remote server...\n");
                raylib_printf("Downloading resources from: %s\n", arg1);
                
                if (strstr(arg1, "codepad.app/pad/822052z5n") != NULL) {
                    FILE *dl = fopen("downloads/flag.txt", "w");
                    if (dl) {
                        fprintf(dl, "SEAL{c_strings_are_pointers_to_fun}\n");
                        fclose(dl);
                        raylib_printf("Saved successfully to downloads/flag.txt\n");
                    }
                } else {
                    FILE *dl = fopen("downloads/index.html", "w");
                    if (dl) {
                        fprintf(dl, "\n<h1>SealOS Sandbox Landing</h1>\n");
                        fclose(dl);
                        raylib_printf("Saved successfully to downloads/index.html\n");
                    }
                }
            }
        }
    }
    else if (strcmp(cmd, "exe") == 0) {
        if (parsed_args < 2) {
            raylib_printf("err: missing script file\n");
        } else {
            char script_path[128];
            get_current_path(loc, arg1, script_path);
            
            FILE *script = fopen(script_path, "r");
            if (!script) {
                raylib_printf("err: script file not found\n");
            } else {
                char script_line[100];
                raylib_printf("---  %s ---\n", arg1);
                while (fgets(script_line, sizeof(script_line), script) != NULL) {
                    script_line[strcspn(script_line, "\r\n")] = 0;
                    if (strlen(script_line) == 0) continue; 
                    
                    raylib_printf("[%s] executing: %s\n", arg1, script_line);
                    process_system_command(script_line); 
                }
                fclose(script);
            }
        }
    }
    else if (strcmp(cmd, "rnm") == 0) { 
        raylib_printf("err: blocking keyboard inputs are handled via screen events.\n");
    }
    else if (strcmp(cmd, "mkfile") == 0) { 
        if (parsed_args < 2) {
            raylib_printf("err: missing name\n");
        } else {
            char folder_path[128];
            sprintf(folder_path, "documents/%s", arg1);
            
            if (mkdir(folder_path, 0777) == 0) {
                raylib_printf("changed\n");
            } else {
                raylib_printf("err: folder unable to generate\n");
            }
        }
    }
    else if (strcmp(cmd, "ls-d") == 0) {
        raylib_printf("prwxf---o---surwx, system/sys.txt\n");
#ifdef _WIN32
        struct _finddata_t c_file;
        intptr_t hFile;
        if ((hFile = _findfirst("documents/*", &c_file)) != -1L) {
            do {
                if (strcmp(c_file.name, ".") != 0 && strcmp(c_file.name, "..") != 0) {
                    if (c_file.attrib & _A_SUBDIR) {
                        raylib_printf("drwxf---o---surwx, documents/%s/\n", c_file.name);
                    } else {
                        if (notes_mode == 1) raylib_printf("prwxf---o---surwx, documents/%s\n", c_file.name);
                        else raylib_printf("p---f---o---surwx, documents/%s\n", c_file.name);
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
                        raylib_printf("drwxf---o---surwx, documents/%s/\n", dir->d_name);
                    } else {
                        if (notes_mode == 1) raylib_printf("prwxf---o---surwx, documents/%s\n", dir->d_name);
                        else raylib_printf("p---f---o---surwx, documents/%s\n", dir->d_name);
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
        get_current_path(loc, notes_name, notes_path);
        file = fopen(notes_path, "w");
        if (file) {
            fprintf(file, "ZL TEXT EDITOR:\n");
            fclose(file);
            
        }
    }
    else if (strcmp(cmd, "r") == 0) {
        if (parsed_args < 2) {
            raylib_printf("err: missing filename\n");
        } else {
            if (strcmp(arg1, notes_name) == 0 && notes_mode == 0) {
                raylib_printf("err: permission denied\n");
            } else {
                char read_path[128], line_buffer[256];
                get_current_path(loc, arg1, read_path);
                FILE *rf = fopen(read_path, "r");
                if (rf) {
                    raylib_printf("--- CONTENT OF %s ---\n", arg1);
                    while (fgets(line_buffer, sizeof(line_buffer), rf) != NULL) {
                        raylib_printf("%s", line_buffer);
                    }
                    fclose(rf);
                } else {
                    raylib_printf("err: file unreadable or missing\n");
                }
            }
        }
    }
    else if (strcmp(cmd, "info") == 0) {
        raylib_printf("                ..^~:::::::::::....              \n");
        raylib_printf("            .::::.......:^::.. ..:..:::.          \n");
        raylib_printf("         ...::..          .:.:.  .::.   .:^:        \n");
        raylib_printf("        ::..              .: ~:. ::..      .::      \n");
        raylib_printf("      ::.   ..^ :^..      .. . :. ...  :     :^.    \n");
        raylib_printf("     .^    P@G:  7&#~      ...         ~       ^.   \n");
        raylib_printf("    :^     ~!::!^.:!^  .   ::. ::::.^  ~. .    .^   \n");
        raylib_printf("    ^:     ~^?B@@B!:~  ^     .:^..:::...:..     !^. \n");
        raylib_printf("   ^:^    ^77GP5PPJ!^  ........ ...  .:   . ^: :^ ~\n");
        raylib_printf("   ^.^:   :...    ^^        ....... :^:^^~:: .^.:^ \n");
        raylib_printf("    ..:::...:^:.  ^. .. .. .^.......^^..^:...:^:..  \n");
        raylib_printf("        ....:::::^^^~~^^^:^~ : ^:::^^.......       \n");
        raylib_printf("                .......^:~^~~^^.               \n");
        raylib_printf("\n");
        raylib_printf("SealOS 7 \n");
        raylib_printf("Code Env.: CodePad\n");
        raylib_printf("PC Info: Virtual\n");
        raylib_printf("Copyleft SealOS from ZL Project\n");
        raylib_printf("2026\n");
    }

    else if (strcmp(cmd, "about") == 0) {
        raylib_printf("                ..^~:::::::::::....              \n");
        raylib_printf("            .::::.......:^::.. ..:..:::.          \n");
        raylib_printf("         ...::..          .:.:.  .::.   .:^:        \n");
        raylib_printf("        ::..              .: ~:. ::..      .::      \n");
        raylib_printf("      ::.   ..^ :^..      .. . :. ...  :     :^.    \n");
        raylib_printf("     .^    P@G:  7&#~      ...         ~       ^.   \n");
        raylib_printf("    :^     ~!::!^.:!^  .   ::. ::::.^  ~. .    .^   \n");
        raylib_printf("    ^:     ~^?B@@B!:~  ^     .:^..:::...:..     !^. \n");
        raylib_printf("   ^:^    ^77GP5PPJ!^  ........ ...  .:   . ^: :^ ~\n");
        raylib_printf("   ^.^:   :...    ^^        ....... :^:^^~:: .^.:^ \n");
        raylib_printf("    ..:::...:^:.  ^. .. .. .^.......^^..^:...:^:..  \n");
        raylib_printf("        ....:::::^^^~~^^^:^~ : ^:::^^.......       \n");
        raylib_printf("                .......^:~^~~^^.               \n");
        raylib_printf("\n");
        raylib_printf("SealOS 7 \n");
        raylib_printf("Code Env.: VM\n");
        raylib_printf("Code Env. 2: CodePad\n");
        raylib_printf("Code: C, C++\n");
        raylib_printf("Host: CodePad Server\n");
        raylib_printf("PC Info: Virtual\n");
        raylib_printf("Copyleft SealOS from ZL Project\n");
        raylib_printf("2026\n");
        raylib_printf("QWERTYUIOPASDFGHJKLZXCVBNM1234567890\n");
    }

    else if (strcmp(cmd, "quick") == 0) {
        raylib_printf("                ..^~:::::::::::....              \n");
        raylib_printf("            .::::.......:^::.. ..:..:::.          \n");
        raylib_printf("         ...::..          .:.:.  .::.   .:^:        \n");
        raylib_printf("        ::..              .: ~:. ::..      .::      \n");
        raylib_printf("      ::.   ..^ :^..      .. . :. ...  :     :^.    \n");
        raylib_printf("     .^    P@G:  7&#~      ...         ~       ^.   \n");
        raylib_printf("    :^     ~!::!^.:!^  .   ::. ::::.^  ~. .    .^   \n");
        raylib_printf("    ^:     ~^?B@@B!:~  ^     .:^..:::...:..     !^. \n");
        raylib_printf("   ^:^    ^77GP5PPJ!^  ........ ...  .:   . ^: :^ ~\n");
        raylib_printf("   ^.^:   :...    ^^        ....... :^:^^~:: .^.:^ \n");
        raylib_printf("    ..:::...:^:.  ^. .. .. .^.......^^..^:...:^:..  \n");
        raylib_printf("        ....:::::^^^~~^^^:^~ : ^:::^^.......       \n");
        raylib_printf("                .......^:~^~~^^.               \n");
        raylib_printf("\n");
        raylib_printf("SealOS 7 \n");
        raylib_printf("Code Env.: CodePad\n");
        raylib_printf("Code: C, C++\n");
        raylib_printf("Host: CodePad Server\n");
        raylib_printf("PC Info: Virtual\n");
        raylib_printf("Copyleft SealOS from ZL Project\n");
        raylib_printf("2026\n");
        raylib_printf("QWERTYUIOPASDFGHJKLZXCVBNM1234567890\n");
    }
    else if (strcmp(cmd, "goto") == 0) {
        if (strcmp(arg1, "home") == 0) loc = 1;
        else if (strcmp(arg1, "documents") == 0) loc = 2;
        else if (strcmp(arg1, "downloads") == 0) loc = 3;
        else if (strcmp(arg1, "system") == 0) loc = 4;
        else raylib_printf("err: unknown folder\n");
    }
    else if (strcmp(cmd, "back") == 0) {
        loc = 0;
    }
    else if (strcmp(cmd, "where") == 0) {
        const char* locations[] = {"root", "home", "documents", "downloads", "system"};
        if (loc >= 0 && loc <= 4) raylib_printf("Current: %s\n", locations[loc]);
    }
    else if (strcmp(cmd, "ls") == 0) {
        raylib_printf("home/  downloads/  documents/  system/\n");
    }
    else if (strcmp(cmd, "help") == 0) {
        raylib_printf("SEALOS SCRIPT LIST:\n");
        raylib_printf("goto (Folder) - goes to 1 folder\n");
        raylib_printf("ls - list folders and files out\n");
        raylib_printf("ls-d - list folders and files out detailed\n");
        raylib_printf("info - show OS specification\n");
        raylib_printf("about - show OS specification\n");
        raylib_printf("quick - show OS specification\n");
        raylib_printf("w notes.txt - create/write a file called notes.txt\n");
        raylib_printf("r notes.txt - read the contents of notes.txt\n"); 
        raylib_printf("rnm notes.txt - rename notes.txt to something else\n");
        raylib_printf("chmod notes.txt 1 - change notes.txt to public\n");
        raylib_printf("chmod notes.txt 0 - change notes.txt to private\n");
        raylib_printf("mkfile (folder name) - create (folder)\n"); 
        raylib_printf("where - to show where you are\n");
        raylib_printf("back - go back to root\n");
        
        raylib_printf("echo (text) - repeat (text)\n");
        raylib_printf("date - show current date and time\n");
        raylib_printf("random - show random numbers\n");
        raylib_printf("rm - remove file\n");
        raylib_printf("save - download files from websites\n");
        raylib_printf("\n");
        raylib_printf("w [name].seal - create a executable script\n");
        raylib_printf("exe [name].seal - run .seal script\n");
    }
    else if (strcmp(cmd, "echo") == 0) {
        raylib_printf("%s\n", input + (strlen(input) > 4 ? 5 : 0));
    }
    else if (strcmp(cmd, "date") == 0) {
        time(&currentTime); 
        raylib_printf("%s", ctime(&currentTime));
    }
    else if (strcmp(cmd, "random") == 0) {
        raylib_printf("Rand value: %d\n", rand());
    }
    else if (strcmp(cmd, "clear") == 0) {
        log_line_count = 0;
    }
    else if (strcmp(cmd, "exit") == 0) {
        CloseWindow();
        exit(0);
    }
    else {
        raylib_printf("err: command not found\n");
    }
}

int main() {
    srand(time(NULL));

    mkdir("home", 0777);
    mkdir("downloads", 0777);
    mkdir("documents", 0777);
    mkdir("system", 0777);


    const int viewportWidth = 1024;
    const int viewportHeight = 640;
    InitWindow(viewportWidth, viewportHeight, "SealOS 7");
    SetTargetFPS(60);

    raylib_printf("Type 'help' for commands.\n\n");
    const char* environment_directory_tags[] = {"root", "home", "documents", "downloads", "system"};

    while (!WindowShouldClose()) {

        int key_code = GetCharPressed();
        while (key_code > 0) {

            if ((key_code >= 32) && (key_code <= 125) && (input_letter_count < 50)) {
                current_input_buffer[input_letter_count] = (char)key_code;
                current_input_buffer[input_letter_count + 1] = '\0';
                input_letter_count++;
            }
            key_code = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            input_letter_count--;
            if (input_letter_count < 0) input_letter_count = 0;
            current_input_buffer[input_letter_count] = '\0';
        }


        if (IsKeyPressed(KEY_ENTER)) {
            if (input_letter_count > 0) {
                char execution_payload[128];
                sprintf(execution_payload, "seal>%s$ %s", environment_directory_tags[loc], current_input_buffer);
                raylib_printf("%s\n", execution_payload);
                

                process_system_command(current_input_buffer);
                

                current_input_buffer[0] = '\0';
                input_letter_count = 0;
            }
        }

        frame_cursor_counter++;

        BeginDrawing();
        ClearBackground(BLACK);


        DrawRectangleLines(10, 10, viewportWidth - 20, viewportHeight - 20, GREEN);
        DrawText("SEALOS TERMINAL", 30, 2, 12, BLACK);
        DrawRectangle(30, 0, 240, 15, BLACK);
        DrawText("SEALOS TERMINAL", 35, 2, 12, GREEN);

        int row_y_placement = 25;
        for (int i = 0; i < log_line_count; i++) {
            DrawText(terminal_log[i], 25, row_y_placement, 18, RAYWHITE);
            row_y_placement += 22;
        }


        DrawRectangle(12, viewportHeight - 45, viewportWidth - 24, 2, DARKGRAY);
        
        char live_prompt_string[128];
        sprintf(live_prompt_string, "seal>%s$ %s", environment_directory_tags[loc], current_input_buffer);
        DrawText(live_prompt_string, 25, viewportHeight - 35, 18, GREEN);

        if (((frame_cursor_counter / 20) % 2) == 0) {
            int input_text_pixel_width = MeasureText(live_prompt_string, 18);
            DrawRectangle(25 + input_text_pixel_width + 2, viewportHeight - 32, 10, 15, GREEN);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
