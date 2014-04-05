#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <stdint.h>

#ifdef	__INT64_TYPE__
#define	INTEGER_64
#endif

struct HIDRaw {
public:
  enum CollectionType {
    Physical = 0,
    Application = 1,	// Mouse or keyboard.
    Logical = 2,	// Inter-related data.
    Report = 3,
    NamedArray = 4,
    UsageSwitch = 5,
    UsageModifier = 6,
    ReservedStart = 7,
    ReservedEnd = 0x7f,
    VendorStart = 0x80,
    VendorEnd = 0xff
  };
 
  enum GenericDesktop {
    Undefined = 0x00,
    Pointer = 0x01,
    Mouse = 0x02,
    Joystick = 0x04,
    Game_Pad = 0x05,
    Keyboard = 0x06,
    Keypad = 0x07,
    Multi_axis_Controller = 0x08,
    X = 0x30,
    Y = 0x31,
    Z = 0x32,
    Rx = 0x33,
    Ry = 0x34,
    Rz = 0x35,
    Slider = 0x36,
    Dial = 0x37,
    Wheel = 0x38,
    Hat_Switch = 0x39,
    Counted_Buffer = 0x3A,
    Byte_Count = 0x3B,
    Motion_Wakeup = 0x3C,
    Vx = 0x40,
    Vy = 0x41,
    Vz = 0x42,
    Vbrx = 0x43,
    Vbry = 0x44,
    Vbrz = 0x45,
    Vno = 0x46,
    Feature_Notification = 0x47,
    System_Control = 0x80,
    System_Power_Down = 0x81,
    System_Sleep = 0x82,
    System_Wake_Up = 0x83,
    System_Context_Menu = 0x84,
    System_Main_Menu = 0x85,
    System_App_Menu = 0x86,
    System_Menu_Help = 0x87,
    System_Menu_Exit = 0x88,
    System_Menu_Select = 0x89,
    System_Menu_Right = 0x8A,
    System_Menu_Left = 0x8B,
    System_Menu_Up = 0x8C,
    System_Menu_Down = 0x8D,
    D_pad_Up = 0x90,
    D_pad_Down = 0x91,
    D_pad_Right = 0x92,
    D_pad_Left = 0x93
  };

  enum GlobalType {
    UsagePage = 0,
    LogicalMinimum = 1,
    LogicalMaximum = 2,
    PhysicalMinimum = 3,
    PhysicalMaximum = 4,
    UnitExponent = 5,
    Unit = 6,
    ReportSize = 7,
    ReportID = 8,
    ReportCount = 9,
    Push = 10,
    Pop = 11
  };

  enum LocalType {
    Usage = 0,
    UsageMinimum = 1,
    UsageMaximum = 2,
    DesignatorIndex = 3,
    DesignatorMinimum = 4,
    DesignatorMaximim = 5,
    StringIndex = 7,
    StringMinimum = 8,
    StringMaximum = 9,
    Delimiter = 10
  };
  enum MainType {
    Input = 0x8,
    Output = 0x9,
    Feature = 0xb,
    Collection = 0xa,
    EndCollection = 0xc,
    LongItem = 0x0f,
  };
  
  enum Type {
    Main = 0,
    Global = 1,
    Local = 2
  };
  
  enum UsagePageTypes {
    GenericDesktop = 1,
    Button = 9
  };

  static const unsigned char SizeMap[4];
  
private:
  union {
    unsigned char	bData[260];
    struct {
      unsigned char	bSize:2;
      enum Type		bType:2;
      unsigned char	bTag:4;
      union {
        struct {
	  unsigned char	data_constant:1;
	  unsigned char	array_variable:1;
          unsigned char	absolute_relative:1;
          unsigned char	wraps:1;
          unsigned char nonLinear:1;
          unsigned char	noPreferredState:1;
          unsigned char	nullState:1;
          unsigned char	isVolatile:1;
          unsigned char bitField_bufferedBytes:1;
        };
        struct {
          unsigned char	bDataSize;
          unsigned char	bLongTag;
        };
      };
    };
  };

  void			check_type(Type t) const {
			  if ( type() != t )
			    throw std::runtime_error("Type confusion");
			}

  std::ostream &	print_flags(std::ostream & stream) const;
  std::ostream &	print_global(std::ostream & stream) const;
  std::ostream &	print_local(std::ostream & stream) const;
  std::ostream &	print_main(std::ostream & stream) const;

  void			numeric_size_error() const {
			    throw std::runtime_error("Number size error");
			}
public:
  bool			longItem() const {
			  return (MainType)bTag == LongItem;
			}

  static const char *	collection_type_to_s(unsigned int type);

  const unsigned char data(const unsigned char index) const {
			  if ( longItem() ) {
                            if ( index > bDataSize )
                              return 0;
                            else
			      return bData[index + 3];
			  }
 			  else {
			    if ( index >= SizeMap[bSize] )
                              return 0;
			    else
			      return bData[index + 1];
			  }
			}

  unsigned char		data_length() const {
 			  if ( longItem() )
                            return bDataSize;
                          else {
			    return SizeMap[bSize];
			  }
                        }

  GlobalType		global_type() const {
			  check_type(Global);
			  return (GlobalType)tag();
			}

  static const char *	global_type_to_s(GlobalType);

  unsigned char		item_length() const {
 			  if ( longItem() )
                            return bDataSize + 3;
                          else
                            return SizeMap[bSize] + 1;
                        }

  LocalType		local_type() const {
			  check_type(Local);
			  return (LocalType)tag();
			}

  static const char *	local_type_to_s(LocalType);

  MainType		main_type() const {
			  check_type(Main);
			  return (MainType)tag();
			}

  static const char *	main_type_to_s(MainType);

#ifdef	INTEGER_64
  int64_t
#else
  int32_t
#endif
			signed_integer() const {
			  switch ( data_length() ) {
                          case 0:
			    return 0;
			  case 1:
			    return (signed char)data(0);
			  case 2:
			    return (signed int16_t)
			       (data(1) << 8         )
			     | (data(0)      & 0x00FF);
                          case 4:
			    return (signed int32_t)
			       (data(3) << 24             )
			     | (data(2) << 16 & 0x00FF0000)
			     | (data(1) << 8  & 0x0000FF00)
			     | (data(0)       & 0x000000FF);
#ifdef INTEGER_64
 			  case 8:
			    return (int64_t)
			       ((int64_t)data(7) << 56                      )
			     | ((int64_t)data(6) << 48 & 0x00FF000000000000L)
			     | ((int64_t)data(5) << 40 & 0x0000FF0000000000L)
			     | ((int64_t)data(4) << 32 & 0x000000FF00000000L)
			     | ((int64_t)data(3) << 24 & 0x00000000FF000000L)
			     | ((int64_t)data(2) << 16 & 0x0000000000FF0000L)
			     | ((int64_t)data(1) << 8  & 0x000000000000FF00L)
			     | ((int64_t)data(0)      & 0x00000000000000FFL);
#endif
			  default:
			    numeric_size_error();
			    return 0;
			  }
			}

#ifdef INTEGER_64
  uint64_t
#else
  uint32_t
#endif
			unsigned_integer() const {
			  switch ( data_length() ) {
                          case 0:
			    return 0;
			  case 1:
			    return data(0);
			  case 2:
			    return
			       (data(1) << 8 & 0xFF00)
			     | (data(0)      & 0x00FF);
                          case 4:
			    return
			       (data(3) << 24 & 0xFF000000)
			     | (data(2) << 16 & 0x00FF0000)
			     | (data(1) << 8  & 0x0000FF00)
			     | (data(0)       & 0x000000FF);
#ifdef INTEGER_64
 			  case 8:
			    return (uint64_t)
			       ((uint64_t)data(7) << 56 & 0xFF00000000000000L)
			     | ((uint64_t)data(6) << 48 & 0x00FF000000000000L)
			     | ((uint64_t)data(5) << 40 & 0x0000FF0000000000L)
			     | ((uint64_t)data(4) << 32 & 0x000000FF00000000L)
			     | ((uint64_t)data(3) << 24 & 0x00000000FF000000L)
			     | ((uint64_t)data(2) << 16 & 0x0000000000FF0000L)
			     | ((uint64_t)data(1) << 8  & 0x000000000000FF00L)
			     | ((uint64_t)data(0)       & 0x00000000000000FFL);
#endif
			  default:
			    std::cerr << "Error: Numeric data with size "
			     << data_length() << '.' << std::endl;
			    return 0;
			  }
			}

  std::ostream &	print(std::ostream & stream) const;

  unsigned char		tag() const {
			  if ( longItem() )
			    return bLongTag;
                          else
                            return bTag;
			}

  enum Type		type() const {
			  return bType;
			}
};

const unsigned char HIDRaw::SizeMap[4] = { 0, 1, 2, 4 };

const char *
HIDRaw::collection_type_to_s(unsigned int type)
{
  static const char *	names[] = {
    "Physical",
    "Application",
    "Logical",
    "Report",
    "NamedArray",
    "UsageSwitch",
    "UsageModifier",
    "ReservedStart",
    "ReservedEnd"
  };

  if ( type >= 0 && type <= sizeof(names) / sizeof(*names) )
    return names[type];
  else
    return "Reserved";
}

const char *
HIDRaw::global_type_to_s(GlobalType type) {
  static const char *	names[] = {
    "UsagePage",
    "LogicalMinimum",
    "LogicalMaximum",
    "PhysicalMinimum",
    "PhysicalMaximum",
    "UnitExponent",
    "Unit",
    "ReportSize",
    "ReportID",
    "ReportCount",
    "Push",
    "Pop"
  };
  if ( type >= UsagePage && type <= Pop )
    return names[(unsigned char)type];
  else
    return "Reserved";
}

const char *
HIDRaw::local_type_to_s(LocalType type) {
  static const char * names[] = {
    "Usage",
    "UsageMinimum",
    "UsageMaximum",
    "DesignatorIndex",
    "DesignatorMinimum",
    "DesignatorMaximim",
    "Reserved",
    "StringIndex",
    "StringMinimum",
    "StringMaximum",
    "Delimiter"
  };
  if ( type >= Usage && type <= Delimiter )
    return names[(unsigned char)type];
  else
    return "Reserved";
}

const char *
HIDRaw::main_type_to_s(MainType type) {
  switch ( type ) {
  case Input:
    return "Input";
  case Output:
    return "Output";
  case Feature:
    return "Feature";
  case Collection:
    return "Collection";
  case EndCollection:
    return "EndCollection";
  default:
    return "Reserved";
  }
}

std::ostream &
HIDRaw::print(std::ostream & stream) const
{
  switch ( type() ) {
  case Main:
    return print_main(stream);
  case Global:
    return print_global(stream);
  case Local:
    return print_local(stream);
  }
}

std::ostream &
HIDRaw::print_flags(std::ostream & stream) const
{
  if ( data_constant )
    stream << "Constant, ";
  else
    stream << "Data, ";
  if ( array_variable )
    stream << "Variable, ";
  else
    stream << "Array, ";
  if ( absolute_relative )
    stream << "Relative, ";
  else
    stream << "Absolute, ";
  if ( wraps )
    stream << "Wraps, ";
  else
    stream << "Does Not Wrap, ";
  if ( nonLinear )
    stream << "NonLinear, ";
  else
    stream << "Linear, ";
  if ( noPreferredState )
    stream << "No Preferred State, ";
  else
    stream << "Has a Preferred State, ";
  if ( nullState )
    stream << "Has a Null Position, ";
  else
    stream << "No Null Position, ";
  if ( isVolatile )
    stream << "Volatile, ";
  else
    stream << "Non Volatile, ";
  if ( bitField_bufferedBytes )
    stream << "Buffered Bytes";
  else
    stream << "Bit Field";
}

std::ostream &
HIDRaw::print_global(std::ostream & stream) const
{
  stream << global_type_to_s(global_type());
  stream << ' ';
  switch ( global_type() ) {
   case LogicalMinimum:
   case LogicalMaximum:
   case PhysicalMinimum:
   case PhysicalMaximum:
   case UnitExponent:
     stream << signed_integer();
     break;
   default:
	 stream << unsigned_integer();
  }
  stream << std::endl;
  return stream;
}

std::ostream &
HIDRaw::print_local(std::ostream & stream) const
{
  stream << local_type_to_s(local_type());

  if ( data_length() > 0 )
    stream << ' ' << unsigned_integer();

  stream << std::endl;

  return stream;
}

std::ostream &
HIDRaw::print_main(std::ostream & stream) const
{
  stream << main_type_to_s(main_type());

  switch ( main_type() ) {
  case Input:
  case Output:
  case Feature:
    stream << ' ';
    print_flags(stream);
    stream << std::endl;
    return stream;

  case Collection:
    stream << ' ' << collection_type_to_s(unsigned_integer()) << std::endl;
    return stream;
  case EndCollection:
    break;
  }

  if ( data_length() > 0 )
    stream << ' ' << unsigned_integer();

  stream << std::endl;

  return stream;
}

main()
{
  static const unsigned char descriptor[] = {
    0x05, 0x01, 0x09, 0x04, 0xA1, 0x01, 0xA1, 0x02,
    0x85, 0x01, 0x75, 0x08, 0x95, 0x01, 0x15, 0x00,
    0x26, 0xFF, 0x00, 0x81, 0x03, 0x75, 0x01, 0x95,
    0x13, 0x15, 0x00, 0x25, 0x01, 0x35, 0x00, 0x45,
    0x01, 0x05, 0x09, 0x19, 0x01, 0x29, 0x13, 0x81,
    0x02, 0x75, 0x01, 0x95, 0x0D, 0x06, 0x00, 0xFF,
    0x81, 0x03, 0x15, 0x00, 0x26, 0xFF, 0x00, 0x05,
    0x01, 0x09, 0x01, 0xA1, 0x00, 0x75, 0x08, 0x95,
    0x04, 0x35, 0x00, 0x46, 0xFF, 0x00, 0x09, 0x30,
    0x09, 0x31, 0x09, 0x32, 0x09, 0x35, 0x81, 0x02,
    0xC0, 0x05, 0x01, 0x75, 0x08, 0x95, 0x27, 0x09,
    0x01, 0x81, 0x02, 0x75, 0x08, 0x95, 0x30, 0x09,
    0x01, 0x91, 0x02, 0x75, 0x08, 0x95, 0x30, 0x09,
    0x01, 0xB1, 0x02, 0xC0, 0xA1, 0x02, 0x85, 0x02,
    0x75, 0x08, 0x95, 0x30, 0x09, 0x01, 0xB1, 0x02,
    0xC0, 0xA1, 0x02, 0x85, 0xEE, 0x75, 0x08, 0x95,
    0x30, 0x09, 0x01, 0xB1, 0x02, 0xC0, 0xA1, 0x02,
    0x85, 0xEF, 0x75, 0x08, 0x95, 0x30, 0x09, 0x01,
    0xB1, 0x02, 0xC0, 0xC0
  };

  for ( unsigned int	i = 0; i < sizeof(descriptor); ) {
    const HIDRaw * raw = (HIDRaw *)&descriptor[i];
    raw->print(std::cout);
    i += raw->item_length();
  }
  return 0;
}