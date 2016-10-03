/*
 * This file is part of Moonlight Embedded.
 *
 * Copyright (C) 2015, 2016 Iwan Timmer
 *
 * Moonlight is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Moonlight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Moonlight; if not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include "audio.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "graphics.h"
#include "input/vita.h"

#define MOONLIGHT_PATH "/moonlight"
#define USER_PATHS "."
#define DEFAULT_CONFIG_DIR "/.config"
#define DEFAULT_CACHE_DIR "/.cache"

#define write_config_string(fd, key, value) fprintf(fd, "%s = %s\n", key, value)
#define write_config_int(fd, key, value) fprintf(fd, "%s = %d\n", key, value)
#define write_config_bool(fd, key, value) fprintf(fd, "%s = %s\n", key, value?"true":"false");

CONFIGURATION config;
char *config_path;

bool inputAdded = false;
static bool mapped = true;
const char* audio_device = NULL;

static struct option long_options[] = {
  {"720", no_argument, NULL, 'a'},
  {"1080", no_argument, NULL, 'b'},
  {"width", required_argument, NULL, 'c'},
  {"height", required_argument, NULL, 'd'},
  {"30fps", no_argument, NULL, 'e'},
  {"60fps", no_argument, NULL, 'f'},
  {"bitrate", required_argument, NULL, 'g'},
  {"packetsize", required_argument, NULL, 'h'},
  {"app", required_argument, NULL, 'i'},
  {"input", required_argument, NULL, 'j'},
  {"mapping", required_argument, NULL, 'k'},
  {"nosops", no_argument, NULL, 'l'},
  {"audio", required_argument, NULL, 'm'},
  {"localaudio", no_argument, NULL, 'n'},
  {"config", required_argument, NULL, 'o'},
  {"platform", required_argument, 0, 'p'},
  {"save", required_argument, NULL, 'q'},
  {"keydir", required_argument, NULL, 'r'},
  {"remote", no_argument, NULL, 's'},
  {"windowed", no_argument, NULL, 't'},
  {"surround", no_argument, NULL, 'u'},
  {"fps", required_argument, NULL, 'v'},
  {"forcehw", no_argument, NULL, 'w'},
  {"forcehevc", no_argument, NULL, 'x'},
  {"unsupported", no_argument, NULL, 'y'},
  {0, 0, 0, 0},
};

char* __get_path(char* name, char* extra_data_dirs) {
  const char *xdg_config_dir = getenv("XDG_CONFIG_DIR");
  const char *home_dir = getenv("HOME");

  if (access(name, R_OK) != -1) {
      return name;
  }

  if (!extra_data_dirs)
    extra_data_dirs = "/usr/share:/usr/local/share";
  if (!xdg_config_dir)
    xdg_config_dir = home_dir;

  char *data_dirs = malloc(strlen(USER_PATHS) + 1 + strlen(xdg_config_dir) + 1 + strlen(home_dir) + 1 + strlen(DEFAULT_CONFIG_DIR) + 1 + strlen(extra_data_dirs) + 2);
  sprintf(data_dirs, USER_PATHS ":%s:%s/" DEFAULT_CONFIG_DIR ":%s/", xdg_config_dir, home_dir, extra_data_dirs);

  char *path = malloc(strlen(data_dirs)+strlen(MOONLIGHT_PATH)+strlen(name)+2);
  if (path == NULL) {
    fprintf(stderr, "Not enough memory\n");
    exit(-1);
  }

  char* data_dir = data_dirs;
  char* end;
  do {
    end = strstr(data_dir, ":");
    int length = end != NULL?end - data_dir:strlen(data_dir);
    memcpy(path, data_dir, length);
    if (path[0] == '/')
      sprintf(path+length, MOONLIGHT_PATH "/%s", name);
    else
      sprintf(path+length, "/%s", name);

    if(access(path, R_OK) != -1) {
      free(data_dirs);
      return path;
    }

    data_dir = end + 1;
  } while (end != NULL);

  free(data_dirs);
  free(path);
  return NULL;
}

static void parse_argument(int c, char* value, PCONFIGURATION config) {
  switch (c) {
  case 'a':
    config->stream.width = 1280;
    config->stream.height = 720;
    break;
  case 'b':
    config->stream.width = 1920;
    config->stream.height = 1080;
    break;
  case 'c':
    config->stream.width = atoi(value);
    break;
  case 'd':
    config->stream.height = atoi(value);
    break;
  case 'e':
    config->stream.fps = 30;
    break;
  case 'f':
    config->stream.fps = 60;
    break;
  case 'g':
    config->stream.bitrate = atoi(value);
    break;
  case 'h':
    config->stream.packetSize = atoi(value);
    break;
  case 'i':
    config->app = value;
    break;
  case 'j':
    if (config->inputsCount >= MAX_INPUTS) {
      perror("Too many inputs specified");
      exit(-1);
    }
    if (config->inputs[config->inputsCount].path != NULL) {
      free(config->inputs[config->inputsCount].path);
    }
    config->inputs[config->inputsCount].path = malloc(sizeof(char) * strlen(value));
    strcpy(config->inputs[config->inputsCount].path, value);
    config->inputs[config->inputsCount].mapping = config->mapping;
    config->inputsCount++;
    inputAdded = true;
    mapped = true;
    break;
  case 'k':
    config->mapping = malloc(sizeof(char) * strlen(value));
    strcpy(config->mapping, value);
    if (config->mapping == NULL) {
      fprintf(stderr, "Unable to open custom mapping file: %s\n", value);
      exit(-1);
    }
    mapped = false;
    break;
  case 'l':
    config->sops = false;
    break;
  case 'm':
    if (audio_device != NULL) {
      free((char*)audio_device);
    }
    audio_device = malloc(sizeof(char) * strlen(value) + 1);
    strcpy((char*)audio_device, value);
    break;
  case 'n':
    config->localaudio = true;
    break;
  case 'o':
    if (!config_file_parse(value, config))
      exit(EXIT_FAILURE);

    break;
  case 'p':
    if (config->platform != NULL) {
      free(config->platform);
    }
    config->platform = malloc(sizeof(char) * strlen(value));
    strcpy(config->platform, value);
    break;
  case 'q':
    if (config->config_file != NULL) {
      free(config->config_file);
    }
    config->config_file = malloc(sizeof(char) * strlen(value));
    strcpy(config->config_file, value);
    break;
  case 'r':
    strcpy(config->key_dir, value);
    break;
  case 's':
    config->stream.streamingRemotely = 1;
    break;
  case 't':
    config->fullscreen = false;
    break;
  case 'u':
    config->stream.audioConfiguration = AUDIO_CONFIGURATION_51_SURROUND;
    break;
  case 'v':
    config->stream.fps = atoi(value);
    break;
  case 'w':
    config->forcehw = true;
    break;
  case 'x':
    config->stream.supportsHevc = true;
    break;
  case 'y':
    config->unsupported_version = true;
    break;
  case 1:
    if (config->action == NULL) {
      config->action = malloc(sizeof(char) * strlen(value));
      strcpy(config->action, value);
    } else if (config->address == NULL) {
      config->address = malloc(sizeof(char) * strlen(value));
      strcpy(config->address, value);
    } else {
      perror("Too many options");
      exit(-1);
    }
  }
}

bool config_file_parse(char* filename, PCONFIGURATION config) {
  FILE* fd = fopen(filename, "r");
  if (fd == NULL) {
    printf("Can't open configuration file: %s\n", filename);
    return false;
  }

  char *line = NULL;
  char value[256];
  size_t len = 0;

  while (__getline(&line, &len, fd) != -1) {
    char key[256], scan_value[256];
    memset(value, 0, 256);
    if (sscanf(line, "%256s = %256s\n", &key, &scan_value) == 2) {
      strcpy(value, scan_value);
      if (strcmp(key, "address") == 0) {
        config->address = malloc(sizeof(char) * strlen(value));
        strcpy(config->address, value);
      } else if (strcmp(key, "sops") == 0) {
        config->sops = strcmp("true", value) == 0;
      } else if (strcmp(key, "localaudio") == 0) {
        config->localaudio = strcmp("true", value) == 0;
      } else if (strcmp(key, "backtouchscreen_deadzone") == 0) {
        sscanf(value, 
            "%d,%d,%d,%d", 
            &config->back_deadzone.top, 
            &config->back_deadzone.right, 
            &config->back_deadzone.bottom, 
            &config->back_deadzone.left);
      } else if (strcmp(key, "special_keys") == 0) {
        sscanf(value,
            "%x,%x,%x,%x,%d,%d",
            &config->special_keys.nw,
            &config->special_keys.ne,
            &config->special_keys.sw,
            &config->special_keys.se,
            &config->special_keys.offset,
            &config->special_keys.size);
      } else if (strcmp(key, "disable_powersave") == 0) {
        config->disable_powersave = strcmp("true", value) == 0;
      } else {
        for (int i=0;long_options[i].name != NULL;i++) {
          if (long_options[i].has_arg == required_argument && strcmp(long_options[i].name, key) == 0) {
            parse_argument(long_options[i].val, value, config);
          }
        }
      }
    }
  }
  return true;
}

void config_save(char* filename, PCONFIGURATION config) {
  FILE* fd = fopen(filename, "w");
  if (fd == NULL) {
    fprintf(stderr, "Can't open configuration file: %s\n", filename);
    exit(EXIT_FAILURE);
  }

  if (config->address)
    write_config_string(fd, "address", config->address);

  if (config->mapping)
    write_config_string(fd, "mapping", config->mapping);

  if (config->stream.width != 1280)
    write_config_int(fd, "width", config->stream.width);
  if (config->stream.height != 720)
    write_config_int(fd, "height", config->stream.height);
  if (config->stream.fps != 60)
    write_config_int(fd, "fps", config->stream.fps);
  if (config->stream.bitrate != -1)
    write_config_int(fd, "bitrate", config->stream.bitrate);
  if (config->stream.packetSize != 1024)
    write_config_int(fd, "packetsize", config->stream.packetSize);
  if (!config->sops)
    write_config_bool(fd, "sops", config->sops);
  if (config->localaudio)
    write_config_bool(fd, "localaudio", config->localaudio);

  if (strcmp(config->app, "Steam") != 0)
    write_config_string(fd, "app", config->app);

  write_config_bool(fd, "disable_powersave", config->disable_powersave);

  char value[256];
  sprintf(
      value,
      "%d,%d,%d,%d",
      config->back_deadzone.top,
      config->back_deadzone.right,
      config->back_deadzone.bottom,
      config->back_deadzone.left);
  write_config_string(fd, "backtouchscreen_deadzone", value);

  sprintf(
      value,
      "%x,%x,%x,%x,%d,%d",
      config->special_keys.nw,
      config->special_keys.ne,
      config->special_keys.sw,
      config->special_keys.se,
      config->special_keys.offset,
      config->special_keys.size);
  write_config_string(fd, "special_keys", value);

  fclose(fd);
}

void config_parse(int argc, char* argv[], PCONFIGURATION config) {
  LiInitializeStreamConfiguration(&config->stream);

  config->stream.width = 1280;
  config->stream.height = 720;
  config->stream.fps = 60;
  config->stream.bitrate = -1;
  config->stream.packetSize = 1024;
  config->stream.streamingRemotely = 0;
  config->stream.audioConfiguration = AUDIO_CONFIGURATION_STEREO;
  config->stream.supportsHevc = false;

  config->platform = "default";
  config->app = "Steam";
  config->action = NULL;
  config->address = NULL;
  config->config_file = NULL;
  config->sops = true;
  config->localaudio = false;
  config->fullscreen = true;
  config->unsupported_version = false;
  config->disable_powersave = true;

  config->special_keys.nw = INPUT_SPECIAL_KEY_PAUSE | INPUT_TYPE_SPECIAL;
  config->special_keys.sw = SPECIAL_FLAG | INPUT_TYPE_GAMEPAD;
  config->special_keys.offset = 0;
  config->special_keys.size = 150;

  config->inputsCount = 0;
  //config->mapping = get_path("mappings/default.conf", getenv("XDG_DATA_DIRS"));
  config->key_dir[0] = 0;

  //char* config_file = get_path("moonlight.conf", "ux0:data/moonlight/");
  char* config_file = config_path;
  if (config_file) {
    config_file_parse(config_file, config);
  }
  if (argc == 2 && access(argv[1], F_OK) == 0) {
    config->action = "stream";
    if (!config_file_parse(argv[1], config))
      exit(EXIT_FAILURE);

  } else {
    int option_index = 0;
    int c;
    while ((c = getopt_long_only(argc, argv, "-abc:d:efg:h:i:j:k:lm:no:p:q:r:stuv:w:xy", long_options, &option_index)) != -1) {
      parse_argument(c, optarg, config);
    }
  }

  if (config->config_file != NULL)
    config_save(config->config_file, config);

  if (config->key_dir[0] == 0x0) {
    const char *xdg_cache_dir = getenv("XDG_CACHE_DIR");
    if (xdg_cache_dir != NULL)
      sprintf(config->key_dir, "%s" MOONLIGHT_PATH, xdg_cache_dir);
    else {
      const char *home_dir = getenv("HOME");
      sprintf(config->key_dir, "%s" DEFAULT_CACHE_DIR MOONLIGHT_PATH, home_dir);
    }
  }

  if (config->stream.bitrate == -1) {
    if (config->stream.height >= 1080 && config->stream.fps >= 60)
      config->stream.bitrate = 20000;
    else if (config->stream.height >= 1080 || config->stream.fps >= 60)
      config->stream.bitrate = 10000;
    else
      config->stream.bitrate = 5000;
  }

  if (inputAdded) {
    if (!mapped) {
        fprintf(stderr, "Mapping option should be followed by the input to be mapped.\n");
        exit(-1);
    } else if (config->mapping == NULL) {
        fprintf(stderr, "Please specify mapping file as default mapping could not be found.\n");
        exit(-1);
    }
  }
}
