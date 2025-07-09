# generated from rosidl_generator_py/resource/_idl.py.em
# with input from motor_msg:msg/Motor.idl
# generated code does not contain a copyright notice


# Import statements for member types

import builtins  # noqa: E402, I100

import math  # noqa: E402, I100

import rosidl_parser.definition  # noqa: E402, I100


class Metaclass_Motor(type):
    """Metaclass of message 'Motor'."""

    _CREATE_ROS_MESSAGE = None
    _CONVERT_FROM_PY = None
    _CONVERT_TO_PY = None
    _DESTROY_ROS_MESSAGE = None
    _TYPE_SUPPORT = None

    __constants = {
    }

    @classmethod
    def __import_type_support__(cls):
        try:
            from rosidl_generator_py import import_type_support
            module = import_type_support('motor_msg')
        except ImportError:
            import logging
            import traceback
            logger = logging.getLogger(
                'motor_msg.msg.Motor')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__msg__motor
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__msg__motor
            cls._CONVERT_TO_PY = module.convert_to_py_msg__msg__motor
            cls._TYPE_SUPPORT = module.type_support_msg__msg__motor
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__msg__motor

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
        }


class Motor(metaclass=Metaclass_Motor):
    """Message class 'Motor'."""

    __slots__ = [
        '_id',
        '_speed',
        '_dir',
        '_mode',
        '_angle',
        '_state',
        '_sub_divide',
    ]

    _fields_and_field_types = {
        'id': 'uint8',
        'speed': 'float',
        'dir': 'uint8',
        'mode': 'uint8',
        'angle': 'float',
        'state': 'uint8',
        'sub_divide': 'uint8',
    }

    SLOT_TYPES = (
        rosidl_parser.definition.BasicType('uint8'),  # noqa: E501
        rosidl_parser.definition.BasicType('float'),  # noqa: E501
        rosidl_parser.definition.BasicType('uint8'),  # noqa: E501
        rosidl_parser.definition.BasicType('uint8'),  # noqa: E501
        rosidl_parser.definition.BasicType('float'),  # noqa: E501
        rosidl_parser.definition.BasicType('uint8'),  # noqa: E501
        rosidl_parser.definition.BasicType('uint8'),  # noqa: E501
    )

    def __init__(self, **kwargs):
        assert all('_' + key in self.__slots__ for key in kwargs.keys()), \
            'Invalid arguments passed to constructor: %s' % \
            ', '.join(sorted(k for k in kwargs.keys() if '_' + k not in self.__slots__))
        self.id = kwargs.get('id', int())
        self.speed = kwargs.get('speed', float())
        self.dir = kwargs.get('dir', int())
        self.mode = kwargs.get('mode', int())
        self.angle = kwargs.get('angle', float())
        self.state = kwargs.get('state', int())
        self.sub_divide = kwargs.get('sub_divide', int())

    def __repr__(self):
        typename = self.__class__.__module__.split('.')
        typename.pop()
        typename.append(self.__class__.__name__)
        args = []
        for s, t in zip(self.__slots__, self.SLOT_TYPES):
            field = getattr(self, s)
            fieldstr = repr(field)
            # We use Python array type for fields that can be directly stored
            # in them, and "normal" sequences for everything else.  If it is
            # a type that we store in an array, strip off the 'array' portion.
            if (
                isinstance(t, rosidl_parser.definition.AbstractSequence) and
                isinstance(t.value_type, rosidl_parser.definition.BasicType) and
                t.value_type.typename in ['float', 'double', 'int8', 'uint8', 'int16', 'uint16', 'int32', 'uint32', 'int64', 'uint64']
            ):
                if len(field) == 0:
                    fieldstr = '[]'
                else:
                    assert fieldstr.startswith('array(')
                    prefix = "array('X', "
                    suffix = ')'
                    fieldstr = fieldstr[len(prefix):-len(suffix)]
            args.append(s[1:] + '=' + fieldstr)
        return '%s(%s)' % ('.'.join(typename), ', '.join(args))

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return False
        if self.id != other.id:
            return False
        if self.speed != other.speed:
            return False
        if self.dir != other.dir:
            return False
        if self.mode != other.mode:
            return False
        if self.angle != other.angle:
            return False
        if self.state != other.state:
            return False
        if self.sub_divide != other.sub_divide:
            return False
        return True

    @classmethod
    def get_fields_and_field_types(cls):
        from copy import copy
        return copy(cls._fields_and_field_types)

    @builtins.property  # noqa: A003
    def id(self):  # noqa: A003
        """Message field 'id'."""
        return self._id

    @id.setter  # noqa: A003
    def id(self, value):  # noqa: A003
        if __debug__:
            assert \
                isinstance(value, int), \
                "The 'id' field must be of type 'int'"
            assert value >= 0 and value < 256, \
                "The 'id' field must be an unsigned integer in [0, 255]"
        self._id = value

    @builtins.property
    def speed(self):
        """Message field 'speed'."""
        return self._speed

    @speed.setter
    def speed(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'speed' field must be of type 'float'"
            assert not (value < -3.402823466e+38 or value > 3.402823466e+38) or math.isinf(value), \
                "The 'speed' field must be a float in [-3.402823466e+38, 3.402823466e+38]"
        self._speed = value

    @builtins.property  # noqa: A003
    def dir(self):  # noqa: A003
        """Message field 'dir'."""
        return self._dir

    @dir.setter  # noqa: A003
    def dir(self, value):  # noqa: A003
        if __debug__:
            assert \
                isinstance(value, int), \
                "The 'dir' field must be of type 'int'"
            assert value >= 0 and value < 256, \
                "The 'dir' field must be an unsigned integer in [0, 255]"
        self._dir = value

    @builtins.property
    def mode(self):
        """Message field 'mode'."""
        return self._mode

    @mode.setter
    def mode(self, value):
        if __debug__:
            assert \
                isinstance(value, int), \
                "The 'mode' field must be of type 'int'"
            assert value >= 0 and value < 256, \
                "The 'mode' field must be an unsigned integer in [0, 255]"
        self._mode = value

    @builtins.property
    def angle(self):
        """Message field 'angle'."""
        return self._angle

    @angle.setter
    def angle(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'angle' field must be of type 'float'"
            assert not (value < -3.402823466e+38 or value > 3.402823466e+38) or math.isinf(value), \
                "The 'angle' field must be a float in [-3.402823466e+38, 3.402823466e+38]"
        self._angle = value

    @builtins.property
    def state(self):
        """Message field 'state'."""
        return self._state

    @state.setter
    def state(self, value):
        if __debug__:
            assert \
                isinstance(value, int), \
                "The 'state' field must be of type 'int'"
            assert value >= 0 and value < 256, \
                "The 'state' field must be an unsigned integer in [0, 255]"
        self._state = value

    @builtins.property
    def sub_divide(self):
        """Message field 'sub_divide'."""
        return self._sub_divide

    @sub_divide.setter
    def sub_divide(self, value):
        if __debug__:
            assert \
                isinstance(value, int), \
                "The 'sub_divide' field must be of type 'int'"
            assert value >= 0 and value < 256, \
                "The 'sub_divide' field must be an unsigned integer in [0, 255]"
        self._sub_divide = value
