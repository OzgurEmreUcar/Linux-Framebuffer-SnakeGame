#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

int main() {
    struct input_event ev;
    int fd;
    int rc;

    // Open the input device (replace eventX with the correct device)
    fd = open("/dev/input/event3", O_RDONLY); // Replace eventX with the correct event number
    if (fd == -1) {
        perror("Failed to open input device");
        return 1;
    }

    printf("Capturing events. Press 'q' to quit.\n");

    // Read events in an infinite loop
    while (1) {
        // Read an event from the input device
        rc = read(fd, &ev, sizeof(struct input_event));
        if (rc < (int) sizeof(struct input_event)) {
            perror("Error reading event");
            break;
        }

        // Check for key events (ev.type == EV_KEY)
        if (ev.type == EV_KEY) {
            if (ev.value == 0) {
                printf("Key released: %d\n", ev.code);  // ev.code is the keycode
            } else if (ev.value == 1) {
                printf("Key pressed: %d\n", ev.code);  // ev.code is the keycode
            }
        }

        // Exit the loop if the 'q' key is pressed
        if (ev.type == EV_KEY && ev.code == KEY_Q && ev.value == 1) {
            printf("Quitting...\n");
            break;
        }
    }

    // Close the file descriptor
    close(fd);

    return 0;
}
