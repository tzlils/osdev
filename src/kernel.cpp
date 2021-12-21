#include <stdint.h>
#include <stddef.h>
#include <stivale2.h>

#include <stdlib/stdio.h>
#include <stdlib/string.h>

#include <mem/gdt.h>
#include <sys/cpuid.h>

// We need to tell the stivale bootloader where we want our stack to be.
// We are going to allocate our stack as an uninitialised array in .bss.
static uint8_t stack[8192];

// stivale2 uses a linked list of tags for both communicating TO the
// bootloader, or receiving info FROM it. More information about these tags
// is found in the stivale2 specification.

// stivale2 offers a runtime terminal service which can be ditched at any
// time, but it provides an easy way to print out to graphical terminal,
// especially during early boot.
// Read the notes about the requirements for using this feature below this
// code block.
static struct stivale2_header_tag_terminal terminal_hdr_tag = {
	// All tags need to begin with an identifier and a pointer to the next tag.
	.tag = {
		// Identification constant defined in stivale2.h and the specification.
		.identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
		// If next is 0, it marks the end of the linked list of header tags.
		.next = 0},
	// The terminal header tag possesses a flags field, leave it as 0 for now
	// as it is unused.
	.flags = 0};

// We are now going to define a framebuffer header tag.
// This tag tells the bootloader that we want a graphical framebuffer instead
// of a CGA-compatible text mode. Omitting this tag will make the bootloader
// default to text mode, if available.
static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
	// Same as above.
	.tag = {
		.identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
		// Instead of 0, we now point to the previous header tag. The order in
		// which header tags are linked does not matter.
		.next = (uint64_t)&terminal_hdr_tag},
	// We set all the framebuffer specifics to 0 as we want the bootloader
	// to pick the best it can.
	.framebuffer_width = 0,
	.framebuffer_height = 0,
	.framebuffer_bpp = 0};

// The stivale2 specification says we need to define a "header structure".
// This structure needs to reside in the .stivale2hdr ELF section in order
// for the bootloader to find it. We use this __attribute__ directive to
// tell the compiler to put the following structure in said section.
__attribute__((section(".stivale2hdr"), used)) static struct stivale2_header stivale_hdr = {
	// The entry_point member is used to specify an alternative entry
	// point that the bootloader should jump to instead of the executable's
	// ELF entry point. We do not care about that so we leave it zeroed.
	.entry_point = 0,
	// Let's tell the bootloader where our stack is.
	// We need to add the sizeof(stack) since in x86(_64) the stack grows
	// downwards.
	.stack = (uintptr_t)stack + sizeof(stack),
	// Bit 1, if set, causes the bootloader to return to us pointers in the
	// higher half, which we likely want.
	// Bit 2, if set, tells the bootloader to enable protected memory ranges,
	// that is, to respect the ELF PHDR mandated permissions for the executable's
	// segments.
	.flags = (1 << 1) | (1 << 2),
	// This header structure is the root of the linked list of header tags and
	// points to the first one in the linked list.
	.tags = (uintptr_t)&framebuffer_hdr_tag};

// We will now write a helper function which will allow us to scan for tags
// that we want FROM the bootloader (structure tags).
void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id)
{
	struct stivale2_tag *current_tag = (struct stivale2_tag *)stivale2_struct->tags;
	for (;;)
	{
		// If the tag pointer is NULL (end of linked list), we did not find
		// the tag. Return NULL to signal this.
		if (current_tag == NULL)
		{
			return NULL;
		}

		// Check whether the identifier matches. If it does, return a pointer
		// to the matching tag.
		if (current_tag->identifier == id)
		{
			return current_tag;
		}

		// Get a pointer to the next tag in the linked list and repeat.
		current_tag = (struct stivale2_tag *)current_tag->next;
	}
}

const uint64_t gdt_entries[9] = {
	0x0000000000000000, // null
	/*
	note: should be able to get rid of {code,data}{16,32} once no longer using stivale terminal
	*/
	0x00009a000000ffff, // code16
	0x000093000000ffff, // data16
	0x00cf9a000000ffff, // code32
	0x00cf93000000ffff, // data32
	0x00af9b000000ffff, // code64
	0x00af93000000ffff, // data64
	0x00affb000000ffff, // usrc64
	0x00aff3000000ffff	// usrd64
};

gdtr_t gdt = {sizeof(gdt_entries) - 1, (uint64_t *)gdt_entries};

[[noreturn]] extern "C" void _start(struct stivale2_struct *stivale2_struct)
{
	// Let's get the terminal structure tag from the bootloader.
	struct stivale2_struct_tag_terminal *term_str_tag;
	term_str_tag = (struct stivale2_struct_tag_terminal *)stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_TERMINAL_ID);

	struct stivale2_struct_tag_framebuffer *fb;
	fb = (struct stivale2_struct_tag_framebuffer *)stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);

	// Check if the tag was actually found.
	if (term_str_tag == NULL || fb == NULL)
	{
		// It wasn't found, just hang...
		for (;;)
		{
			asm("hlt");
		}
	}

	// Let's get the address of the terminal write function.
	void *term_write_ptr = (void *)term_str_tag->term_write;

	// Now, let's assign this pointer to a function pointer which
	// matches the prototype described in the stivale2 specification for
	// the stivale2_term_write function.
	typedef void write_func(const char *, size_t);
	auto term_write = (write_func *)term_write_ptr;

	// We should now be able to call the above function pointer to print out
	// a simple "Hello World" to screen.
	char buffer[512] = "Hello World";
	int bytes_written = 0;
	// bytes_written = snprintf(
	// 	buffer, 512, "version % built at %",
	// 	BUILD_VERSION, BUILD_DATE);
	term_write(buffer, 512);

	// char cpuid_vendor[16];
	// cpuid_string(0, (uint32_t *)cpuid_vendor);
	// bytes_written = vsnprintf(
	// 	buffer, 512, "CPUID vendor: %s\n", cpuid_vendor);
	// term_write(buffer, bytes_written);

	// term_write("Initializing GDT\n", 17);
	// set_gdt(gdt);

	// const char *format = "vsnprintf test\n%%x: %x\n%%d: %d\n%%b: %b\n%%s: %s\n\nformat used:\n\n%s";
	// bytes_written = vsnprintf(buffer, 512, format,
	// 						  3735928559, 3735928559, 3735928559, "FOOBAR", format);
	// term_write(buffer, bytes_written);
	// We're done, just hang...
	for (;;)
	{
		asm("hlt");
	}
}