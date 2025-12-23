#ifndef CM_STRING_C
#define CM_STRING_C

#include <cm_macro_defs.c>
#include <cm_types.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <cm_allocator.c>

#define PRINT_BUFFER_SIZE 10 * 1024

#if (CM_WINDOWS)

#pragma warning(push, 0)
#include <shlwapi.h>
#pragma warning(pop)

i64
write_file(HANDLE h, void* buffer, u32 length)
{
  DWORD written = 0;
  BOOL  value   = WriteFile(h, buffer, length, &written, NULL);
  if (!value) return -1;
  return written;
}

i32
message_box(char* msg)
{
	DWORD written = 0;
	u32 size = (u32)strlen(msg);
	WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), msg, size, &written, NULL);
  i32 value = 1;
	value = MessageBox(NULL, msg, NULL, MB_OK);
	return value;
}

i32
printb(char* fmt, ...)
{
  char buffer[PRINT_BUFFER_SIZE];
  memset(buffer, 0, PRINT_BUFFER_SIZE);
  va_list args;
  va_start(args, fmt);
  i32 len = wvnsprintf(buffer, PRINT_BUFFER_SIZE, fmt, args);
  if (len >= PRINT_BUFFER_SIZE) message_box("Print buffer size was exceeded!");
  va_end(args);
  return message_box(buffer);
}

#elif (OS_LINUX) || (OS_MAC)
#include <unistd.h>

i32
write_file(i32 fd, void* buffer, ui32 length)
{
  return write(fd, buffer, length);
}

i32
message_box(MU char* msg)
{
  return -1;
}

#endif // CM_WINDOWS

char*
cstrchr(char* str, char c)
{
  i32 i = 0;
  i32 len = (i32) strlen(str);
  while (i < len)
  {
    if (str[i] == c)
      return str + i;
    i++;
  }
  return NULL;
}

bool
str_ncmp_impl(char* src, ...)
{
  va_list args;
  va_start(args, src);
  char* cmp;
  while ((cmp = va_arg(args, char*)))
  {
    if (!strcmp(src, cmp))
    {
      return true;
    }
  }
  va_end(args);
  return false;
}

#define str_ncmp(str, ...) str_ncmp_impl(str, __VA_ARGS__, NULL)

global u8 utf8_class[32] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,2,2,2,2,3,3,4,5, };

static UnicodeDecode
utf8_decode(u8 *str, u64 max)
{
  u8            byte       = str[0];
  u8            byte_class = utf8_class[byte >> 3];
  UnicodeDecode result     = {1, _u32max};
  switch (byte_class)
  {
    case 1:
    {
      result.codepoint = byte;
    }break;
    case 2:
    {
      if (2 < max)
      {
        u8 cont_byte = str[1];
        if (utf8_class[cont_byte >> 3] == 0)
        {
          result.codepoint  = (byte & _BITMASK5) << 6;
          result.codepoint |= (cont_byte & _BITMASK6);
          result.inc = 2;
        }
      }
    }break;
    case 3:
    {
      if (2 < max)
      {
        u8 cont_byte[2] = {str[1], str[2]};
        if (utf8_class[cont_byte[0] >> 3] == 0 &&
            utf8_class[cont_byte[1] >> 3] == 0)
        {
          result.codepoint  = (byte & _BITMASK4) << 12;
          result.codepoint |= ((cont_byte[0] & _BITMASK6) << 6);
          result.codepoint |= (cont_byte[1] & _BITMASK6);
          result.inc = 3;
        }
      }
    }break;
    case 4:
    {
      if (3 < max)
      {
        u8 cont_byte[3] = {str[1], str[2], str[3]};
        if (utf8_class[cont_byte[0] >> 3] == 0 &&
            utf8_class[cont_byte[1] >> 3] == 0 &&
            utf8_class[cont_byte[2] >> 3] == 0)
        {
          result.codepoint = (byte & _BITMASK3) << 18;
          result.codepoint |= ((cont_byte[0] & _BITMASK6) << 12);
          result.codepoint |= ((cont_byte[1] & _BITMASK6) <<  6);
          result.codepoint |=  (cont_byte[2] & _BITMASK6);
          result.inc = 4;
        }
      }
    }
  }
  return(result);
}

/* NOTE: https://gist.github.com/tommai78101/3631ed1f136b78238e85582f08bdc618 */
void
utf8_to_utf16(u8* utf8_str, i32 utf8_size, i16* utf16_str, i32 utf16_size)
{
	i32  utf8_cursor     = 0;
	i32  utf16_cursor    = 0;
	u8*  utf8_code_unit  = utf8_str;
	i16* utf16_code_unit = utf16_str;

  /* NOTE: 
   *       Check if UTF-16 iterator is less than max output size. 
   *       If true, then check if UTF-8 iterator is less than UTF-8 max string size.
   *       This conditional checking based on order of precedence is intentionally done so it
   *       prevents the while loop from continuing onwards if the iterators are outside of the intended sizes.
   */
	while (*utf8_code_unit && (utf16_cursor < utf16_size || utf8_cursor < utf8_size)) 
	{
		if (*utf8_code_unit < 0x80)
		{
			/* NOTE: 0..127 ASCII range. */
			*utf16_code_unit = (i16) (*utf8_code_unit);
			utf16_code_unit++;
			utf16_cursor++;

			utf8_code_unit++;
			utf8_cursor++;
		}
		else if (*utf8_code_unit < 0xC0)
		{
			/* NOTE: 0x80..0xBF -> ignore. Reserved for UTF-8 encoding. */
			utf8_code_unit++;
			utf8_cursor++;
		}
		else if (*utf8_code_unit < 0xE0)
		{
			/* NOTE: 128..2047, the extended ASCII range, and into the Basic Multilingual Plane. */
			i16 high_short = (i16) ((*utf8_code_unit) & 0x1F);
			utf8_code_unit++;

			i16 low_short = (i16) ((*utf8_code_unit) & 0x3F);
			utf8_code_unit++;

      /* NOTE:
       *      Need 6 instead of 8.
       *      It's because 0x3F is 6 bits of information from the low short. By shifting 8 bits, you are 
			 *      a2 extra zeroes in between the actual data of both shorts.
       */
			i32 unicode = (high_short << 6) | low_short;

			if ((0 <= unicode && unicode <= 0xD7FF) || (0xE000 <= unicode && unicode <= 0xFFFF))
			{
				*utf16_code_unit = (i16) unicode;
				utf16_code_unit++;
				utf16_cursor++;
			}
			utf8_cursor += 2;
		}
		else if (*utf8_code_unit < 0xF0)
		{
      /* 2048..65535, the remaining Basic Multilingual Plane.
       * 
       * NOTE:
			 *      1110aaaa 10bbbbcc 10ccdddd
			 *      Where a is 4th byte, b is 3rd byte, c is 2nd byte, and d is 1st byte.
       */
			i16 fourth = (i16) ((*utf8_code_unit) & 0xF);
			utf8_code_unit++;

			i16 third = (i16) ((*utf8_code_unit) & 0x3C) >> 2;
			i16 second_high = (i16) ((*utf8_code_unit) & 0x3);
			utf8_code_unit++;

			i16 second_low = (i16) ((*utf8_code_unit) & 0x30) >> 4;
			i16 first = (i16) ((*utf8_code_unit) & 0xF);
			utf8_code_unit++;

			/* NOTE: Create resulting UTF-16 code unit, then increment iterator. */
			i32 unicode = (fourth << 12) | (third << 8) | (second_high << 6) | (second_low << 4) | first;

			/* NOTE: According to math, UTF-8 encoded "unicode" should always fall within these two ranges. */
			if ((0 <= unicode && unicode <= 0xD7FF) || (0xE000 <= unicode && unicode <= 0xFFFF))
			{
				*utf16_code_unit = (i16) unicode;
				utf16_code_unit++;
				utf16_cursor++;
			}
			utf8_cursor += 3;
		}
		else if (*utf8_code_unit < 0xF8)
		{ 
      /* 65536..10FFFF, Unicode UTF range
       *
			 * NOTE:
			 *       11110abb 10bbcccc 10ddddee 10eeffff
			 *       Where a is 6th byte, b is 5th byte, c is 4th byte, and so on.
       */
			i16 sixth         = (i16) ((*utf8_code_unit) & 0x4) >> 2;
			i16 fifth_high    = (i16) ((*utf8_code_unit) & 0x3);
			utf8_code_unit++;

			i16 fifth_low     = (i16) ((*utf8_code_unit) & 0x30) >> 4;
			i16 fourth        = (i16) ((*utf8_code_unit) & 0xF);
			utf8_code_unit++;

			i16 third         = (i16) ((*utf8_code_unit) & 0x3C) >> 2;
			i16 second_high   = (i16) ((*utf8_code_unit) & 0x3);
			utf8_code_unit++;

			i16 second_low    = (i16) ((*utf8_code_unit) & 0x30) >> 4;
			i16 first         = (i16) ((*utf8_code_unit) & 0xF);
			utf8_code_unit++;

			i32 unicode        = (sixth << 4) | (fifth_high << 2) | fifth_low | (fourth << 12) | (third << 8)
                           | (second_high << 6) | (second_low << 4) | first;
			i16 high_surrogate = (i16) (unicode - 0x10000) / 0x400 + 0xD800;
			i16 low_surrogate  = (i16) (unicode - 0x10000) % 0x400 + 0xDC00;

			*utf16_code_unit = low_surrogate;
			utf16_code_unit++;
			utf16_cursor++;

			if (utf16_cursor < utf16_size)
			{
				*utf16_code_unit = high_surrogate;
				utf16_code_unit++;
				utf16_cursor++;
			}
			utf8_cursor += 4;
		}
		else
		{ 
      /* NOTE: Invalid UTF-8 code unit */
			utf8_code_unit++;
			utf8_cursor++;
		}
	}

	/* NOTE: Clean up output string if UTF-16 iterator is still less than output string size. */
	while (utf16_cursor < utf16_size)
	{
		*utf16_code_unit = '\0';
		utf16_code_unit++;
		utf16_cursor++;
	}
}

static S32
s32_from_s8(S8 from, u32 *str32)
{
  S32 result = {0};
  if (from.len == 0) return result;

  u8  *ptr_from = (u8*)from.str;
  u64 size      = 0;
  u8  *opl      = ptr_from + from.len;
  UnicodeDecode consume;
  for(; ptr_from < opl; ptr_from += consume.inc)
  {
    consume = utf8_decode(ptr_from, opl - ptr_from);
    str32[size] = consume.codepoint;
    size += 1;
  }
  str32[size] = 0;
  result.str  = str32;
  result.len  = size;
  return result;
}

static size_t
wstrlen(wchar_t* wstr)
{
	if (!wstr) return 0;
	size_t i;

	i = 0;
	while (wstr[i]) i++;
	return i;
}

/* NOTE: Returns len - 1 */
static uint64_t
wchar_to_char(wchar_t *src, char* dest, uint64_t dest_len)
{
  uint64_t i;
  wchar_t cp;

  i = 0;
  while (src[i] && i < (dest_len - 1))
	{
    cp = src[i];
    if (cp < 128) dest[i] = (char)cp;
    else
		{
      dest[i] = '?';
			// NOTE: lead surrogate, skip the next codepoint (trail)
      if (cp >= 0xD800 && cp <= 0xDBFF) i++;
    }
    i++;
  }
  dest[i] = 0;
  return i - 1;
}

static i32
buflen_until_int(void* buf, i32 c)
{
  i32 len = 0;
  char* str = (char*)buf;
  while (str[len] != c)
  {
    len++;
  }
  return len;
}

static i32
buflen_until_int_n(void* buf, i32 c, i32 size)
{
  i32 len = 0;
  char* str = (char*)buf;
  while (len < size && str[len] != c)
  {
    len++;
  }
  return len;
}

static size_t
c_strlen(char* str)
{
  size_t len = 0;
  while (str[len])
  {
    len++;
  }
  return len;
}

force_inline size_t
string_len(S8 str)
{
  return str.len;
}

force_inline S8
make_string(char* str)
{
  return (S8){.str = (u8*)str, .len = c_strlen(str)};
}
/* 
 * WARN: 
 *       This is unsafe and should be used with trusted input only.
 *       1) 0 to INT_MAX for positive sign
 *       2) Either string is empty or only contains '0' to 9' char
 */ 
static i32
fast_atoi(char* str)
{
  i32 val  = 0;
  i32 sign = 1;
  if (*str++ == '-')
  {
    sign *= -1;
  }
  while(*str)
  {
    val = val * 10 + (*str++ - '0');
  }
  return val * sign;
}

static void
itos(i64 nb, char* buffer, u32 buf_size)
{
  u32 i       = 0;
  u64 size    = 0;
  char nums[] = "0123456789";
  if (nb < 0) { buffer[i] = '-'; nb *= -1; }

  u64 tmp    = nb;
  while (tmp > 0) { tmp/= 10; size++;}
  if (size >= buf_size) return;
  size--;
  if (nb == 0)
  {
    buffer[size - i] = '0';
    return ;
  }
  while (nb > 0)
  {
    buffer[size - i] = nums[(nb % 10)];
    nb /= 10;
    i++;
  }
}

static void
cm_itoa(i64 nb, char* buffer, u32 buf_size)
{
  memset(buffer, 0, buf_size);
  u32 i       = 0;
  u64 size    = 0;
  char nums[] = "0123456789";
  if (nb < 0) { buffer[i] = '-'; nb *= -1; }

  u64 tmp    = nb;
  while (tmp > 0) { tmp/= 10; size++;}
  if (size >= buf_size) return;
  size--;
  if (nb == 0)
  {
    buffer[size - i] = '0';
    return ;
  }
  while (nb > 0)
  {
    buffer[size - i] = nums[(nb % 10)];
    nb /= 10;
    i++;
  }
}

static i32
fast_atoi_n(char* str, i32 size)
{
  i32 val  = 0;
  i32 sign = 1;
  i32 i    = 0;
  if (i < size && str[i] == '-')
  {
    sign *= -1;
    i++;
  }
  while(i < size && (str[i] >= '0' && str[i] <= '9'))
  {
    val = val * 10 + (str[i] - '0');
    i++;
  }
  return val * sign;
}
#define atoi(str) fast_atoi_n((str), (i32)c_strlen((str)))

/* NOTE: This is chatgpt */
static void
reverse(char *str, i32 length)
{
  i32 start = 0;
  i32 end = length - 1;
  while (start < end)
  {
    char temp = str[start];
    str[start] = str[end];
    str[end] = temp;
    start++;
    end--;
  }
}

static char*
ftoa(f32 num, char *str, i32 afterpoint)
{
  // Handle negative numbers
  if (num < 0)
  {
    *str++ = '-';
    num *= -1;
  }
  // Extract integer part
  i32 intPart = (i32)num;
  f32 floatPart = num - intPart;
  // Convert integer part to string
  char intStr[20]; // Buffer for integer part
  i32 i = 0;
  if (intPart == 0)
  {
    intStr[i++] = '0';
  }
  else
  {
    while (intPart)
    {
      intStr[i++] = (intPart % 10) + '0';
      intPart /= 10;
    }
  }
  reverse(intStr, i);
  for (i32 j = 0; j < i; j++)
  {
    *str++ = intStr[j];
  }
  // Process fractional part
  if (afterpoint > 0)
  {
    *str++ = '.'; // Add decimal point
    for (i32 j = 0; j < afterpoint; j++)
    {
      floatPart *= 10;
      i32 fractionalDigit = (i32)floatPart;
      *str++ = (char)fractionalDigit + '0';
      floatPart -= fractionalDigit;
    }
  }
  // Null-terminate the string
  *str = '\0';
  return str;
}


/* WARN: No float in the format ! */
static void
debug_printf_c(char* fmt, ...)
{
  /*
   * NOTE: error LNK2019: unresolved external symbol __chkstk
   *       caused when stack size is `too big`
   * https://www.basicinputoutput.com/2014/08/the-case-of-mysterious-chkstk.html
   */
  char buffer[PRINT_BUFFER_SIZE];
  memset(buffer, 0, PRINT_BUFFER_SIZE);
  va_list args;
  va_start(args, fmt);
  i32 len = wvnsprintf(buffer, PRINT_BUFFER_SIZE, fmt, args);
  if (len >= PRINT_BUFFER_SIZE) MessageBox(NULL, "LEN TOO BIG OUTPUTDEBUG", NULL, MB_OK);
  va_end(args);
  OutputDebugString(buffer);
}

static i64
print_string(S8 string)
{
  return write_file(STDOUT(), string.str, (DWORD)string.len);
}

typedef struct Print_Buffer
{
	u64		cursor;
	u64		total_size;
	char  *buffer;
} Print_Buffer;

#define TLS_PRINT_BUFFER_INIT_SIZE 1024 * 10
#define TLS_PRINT_BUFFER_ITOS  20
#define TLS_PRINT_BUFFER_FTOA  40

_Thread_local bool				 tls_print_is_initialized = false;
_Thread_local char*				 tls_print_buffer_ftoa;
_Thread_local char*				 tls_print_buffer_itos;
_Thread_local Print_Buffer tls_print_buffer;

static inline void
tls_print_buffer_initialize(void)
{
	if (tls_print_is_initialized) return ;
	tls_print_buffer_ftoa =   halloc(TLS_PRINT_BUFFER_FTOA);
	tls_print_buffer_itos =   halloc(TLS_PRINT_BUFFER_ITOS);
	tls_print_buffer.buffer = halloc(TLS_PRINT_BUFFER_INIT_SIZE);
	tls_print_buffer.cursor = 0;
	tls_print_buffer.total_size = TLS_PRINT_BUFFER_INIT_SIZE;
	tls_print_is_initialized = true;
}

static inline void
print_buffer_realloc(u64 new_size)
{
	tls_print_buffer.buffer = hrealloc(tls_print_buffer.buffer, new_size);
	tls_print_buffer.total_size = new_size;
}

static void
print_handle_string(char* str)
{
	u64 len  = strlen(str);
	u64 left = tls_print_buffer.total_size - tls_print_buffer.cursor;
	/* NOTE: Shall we trim it instead ? */
	if (len > left) { print_buffer_realloc(len * 2); }

	memcpy(&tls_print_buffer.buffer[tls_print_buffer.cursor], str, len);
	tls_print_buffer.cursor += len;
}

static void
print_handle_integer(i32 integer)
{
#define ITOA 20
	itos(integer, tls_print_buffer_itos , ITOA);
#undef ITOA
	print_handle_string(tls_print_buffer_itos);
}

static void
print_handle_float(f32 float_nb)
{
	i32 precision = 3;
	tls_print_buffer_ftoa = ftoa(float_nb, tls_print_buffer_ftoa, precision);
	print_handle_string(tls_print_buffer_ftoa);
}

static void
print_handle_other(char c)
{
	char buffer[2];
	buffer[0] = c; buffer[1] = 0;
	print_handle_string(buffer);
}

static i64
print(char* fmt, ...)
{
	cm_assert(tls_print_is_initialized);
	va_list args;
	va_start(args, fmt);

	u64 i = 0;
	while(fmt[i])
	{
		if (fmt[i] == '%')
		{
			i++;
			switch(fmt[i])
			{
				default: i--; goto add_buffer_;
				case 's': print_handle_string(va_arg(args, char*)); break;
				case 'd': print_handle_integer(va_arg(args, int)); break;
				case 'f': print_handle_float(va_arg(args, float)); break;
			}
		}
		else
		{
add_buffer_:;
			print_handle_other(fmt[i]);
		}
		i++;
	}
  return write_file(STDOUT(), tls_print_buffer.buffer, tls_print_buffer.cursor);
}

#ifdef NO_CRT_LINKED
/* WARN: No float in the format ! */
static i64
fprintf(HANDLE h, char* fmt, ...)
{
  char buffer[PRINT_BUFFER_SIZE];
  memset(buffer, 0, PRINT_BUFFER_SIZE);
  va_list args;
  va_start(args, fmt);
  i32 len = wvnsprintf(buffer, PRINT_BUFFER_SIZE, fmt, args);
  if (len >= PRINT_BUFFER_SIZE) MessageBox(NULL, "LEN TOO BIG FPRINTF", NULL, MB_OK);
  va_end(args);
  return write_file(h, buffer, len);
}

/* WARN: No float in the format ! */
static i64
printf(char* fmt, ...)
{
  char buffer[PRINT_BUFFER_SIZE];
  memset(buffer, 0, PRINT_BUFFER_SIZE);
  va_list args;
  va_start(args, fmt);
  i32 len = wvnsprintf(buffer, PRINT_BUFFER_SIZE, fmt, args);
  if (len >= PRINT_BUFFER_SIZE) MessageBox(NULL, "LEN TOO BIG PRINTF", NULL, MB_OK);
  va_end(args);
  return write_file(STDOUT(), buffer, len);
}
#endif

#endif // CM_STRING_C
