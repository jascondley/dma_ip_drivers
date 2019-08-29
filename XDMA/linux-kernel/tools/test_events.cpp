#include <iostream>

#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

constexpr std::size_t READ_SIZE = 4;
int main()
{
  bool running = true;
  int event_count;
  std::size_t bytes_read;
  struct epoll_event event;
  int epoll_fd = epoll_create1(0);
  event.events = EPOLLIN;
  event.data.fd = 0;

  if (epoll_fd == -1) {
    cerr << "Failed to create epoll file descriptor" << endl;
    return 1;
  }

  int event_file = open("/dev/xdma0_events_0", O_RDONLY);
  if (event_file == -1) {
    cerr << "Failed to open xdma evente file" << endl;
    return 1;
  }

  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event_file, &event)) {
    cerr << "Failed to add file descriptor to epoll" << endl;
    return 1;
  }
  char buffer[READ_SIZE] = {'\0'};

  while (running) {
    cout << "Polling for input..." << endl;
    constexpr std::size_t wait_ms = 3000;
    event_count = epoll_wait(epoll_fd, &event, 1, wait_ms);
    cout << "Events ready: " << event_count << endl;
    bytes_read = read(event_file, buffer, READ_SIZE);
    if (bytes_read == -1) {
      cerr << "Error reading events: errno " << errno << endl;
      return 1;
    }
    cout << "Read " << bytes_read << " bytes." << endl;
    cout << "Event contents: " << buffer << endl;
    running = event_count != 0;
  }

  close(event_file);
  close(epoll_fd);
}

