#include "portnotify.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

static const unsigned LINE_LEN = 150;
static const unsigned PORT_OFF = 15;
static const unsigned REM_ADDR_OFF = 20;
static const unsigned MAX_LINES = 200;

static ssize_t get_lines(int cnt, char *buf){
  ssize_t num_read;
  int fd = open("/proc/net/tcp", O_RDONLY);
  num_read = read(fd, buf, cnt*LINE_LEN);
  num_read -= num_read % LINE_LEN;
  buf[num_read] = 0;
  close(fd);
  return num_read/LINE_LEN;
}

static uint16_t get_port(char *line){
  return (uint16_t) strtoul(line+PORT_OFF, NULL, 16);
}

static int rem_addr_is_set(char *line){
  return line[REM_ADDR_OFF] != '0' || line[REM_ADDR_OFF+1] != '0';
}

static uint16_t find_new_port(char *lines, int cnt, uint16_t *old_ports){
  char *cmp;
  int mid = ((cnt+1)/2)-1;

  if(cnt == 1){
    return get_port(lines);
  }

  cmp = lines+mid*LINE_LEN;
  if(get_port(cmp) == old_ports[mid]){
    return find_new_port(cmp+LINE_LEN, cnt-(mid+1), old_ports+(mid+1));
  } else {
    return find_new_port(lines, mid+1, old_ports);
  }
}

uint16_t port_notify(){
  char buf[LINE_LEN * MAX_LINES + 1];
  uint16_t *port_list;
  char *line;
  ssize_t line_cnt;
  ssize_t port_cnt = 0;

  line_cnt = get_lines(MAX_LINES, buf);
  for(int i = 1; i < line_cnt; ++i){
    line = buf+i*LINE_LEN;
    if(rem_addr_is_set(line)){
      assert(i>1);
      port_cnt = i-1;
      break;
    }
  }
  assert(port_cnt > 0);

  port_list = malloc(port_cnt*sizeof(*port_list));
  for(int i = 1; i < port_cnt+1; ++i){
    line = buf+i*LINE_LEN;
    port_list[i-1] = get_port(line);
  }

  while(1){
    line_cnt = get_lines(port_cnt+1+1, buf);
    assert(line_cnt == port_cnt+1+1);
    line = buf+(line_cnt-1)*LINE_LEN;
    if(rem_addr_is_set(line)){
      continue;
    }
    return find_new_port(buf+LINE_LEN, port_cnt+1, port_list);
  }
}

