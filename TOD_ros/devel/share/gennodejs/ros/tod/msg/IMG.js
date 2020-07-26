// Auto-generated. Do not edit!

// (in-package tod.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;
let std_msgs = _finder('std_msgs');

//-----------------------------------------------------------

class IMG {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.data = null;
      this.gtJson_filepath = null;
      this.msglabel = null;
      this.sceneid = null;
      this.imgid = null;
    }
    else {
      if (initObj.hasOwnProperty('data')) {
        this.data = initObj.data
      }
      else {
        this.data = new std_msgs.msg.UInt8MultiArray();
      }
      if (initObj.hasOwnProperty('gtJson_filepath')) {
        this.gtJson_filepath = initObj.gtJson_filepath
      }
      else {
        this.gtJson_filepath = '';
      }
      if (initObj.hasOwnProperty('msglabel')) {
        this.msglabel = initObj.msglabel
      }
      else {
        this.msglabel = '';
      }
      if (initObj.hasOwnProperty('sceneid')) {
        this.sceneid = initObj.sceneid
      }
      else {
        this.sceneid = 0;
      }
      if (initObj.hasOwnProperty('imgid')) {
        this.imgid = initObj.imgid
      }
      else {
        this.imgid = 0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type IMG
    // Serialize message field [data]
    bufferOffset = std_msgs.msg.UInt8MultiArray.serialize(obj.data, buffer, bufferOffset);
    // Serialize message field [gtJson_filepath]
    bufferOffset = _serializer.string(obj.gtJson_filepath, buffer, bufferOffset);
    // Serialize message field [msglabel]
    bufferOffset = _serializer.string(obj.msglabel, buffer, bufferOffset);
    // Serialize message field [sceneid]
    bufferOffset = _serializer.int16(obj.sceneid, buffer, bufferOffset);
    // Serialize message field [imgid]
    bufferOffset = _serializer.int16(obj.imgid, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type IMG
    let len;
    let data = new IMG(null);
    // Deserialize message field [data]
    data.data = std_msgs.msg.UInt8MultiArray.deserialize(buffer, bufferOffset);
    // Deserialize message field [gtJson_filepath]
    data.gtJson_filepath = _deserializer.string(buffer, bufferOffset);
    // Deserialize message field [msglabel]
    data.msglabel = _deserializer.string(buffer, bufferOffset);
    // Deserialize message field [sceneid]
    data.sceneid = _deserializer.int16(buffer, bufferOffset);
    // Deserialize message field [imgid]
    data.imgid = _deserializer.int16(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += std_msgs.msg.UInt8MultiArray.getMessageSize(object.data);
    length += object.gtJson_filepath.length;
    length += object.msglabel.length;
    return length + 12;
  }

  static datatype() {
    // Returns string type for a message object
    return 'tod/IMG';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '35da245cef9bef85956a419b58c6bb86';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    # Data Structure for Image
    # sensor_msgs/Image data
    std_msgs/UInt8MultiArray data
    string gtJson_filepath
    string msglabel
    int16 sceneid
    int16 imgid
    ================================================================================
    MSG: std_msgs/UInt8MultiArray
    # Please look at the MultiArrayLayout message definition for
    # documentation on all multiarrays.
    
    MultiArrayLayout  layout        # specification of data layout
    uint8[]           data          # array of data
    
    
    ================================================================================
    MSG: std_msgs/MultiArrayLayout
    # The multiarray declares a generic multi-dimensional array of a
    # particular data type.  Dimensions are ordered from outer most
    # to inner most.
    
    MultiArrayDimension[] dim # Array of dimension properties
    uint32 data_offset        # padding elements at front of data
    
    # Accessors should ALWAYS be written in terms of dimension stride
    # and specified outer-most dimension first.
    # 
    # multiarray(i,j,k) = data[data_offset + dim_stride[1]*i + dim_stride[2]*j + k]
    #
    # A standard, 3-channel 640x480 image with interleaved color channels
    # would be specified as:
    #
    # dim[0].label  = "height"
    # dim[0].size   = 480
    # dim[0].stride = 3*640*480 = 921600  (note dim[0] stride is just size of image)
    # dim[1].label  = "width"
    # dim[1].size   = 640
    # dim[1].stride = 3*640 = 1920
    # dim[2].label  = "channel"
    # dim[2].size   = 3
    # dim[2].stride = 3
    #
    # multiarray(i,j,k) refers to the ith row, jth column, and kth channel.
    
    ================================================================================
    MSG: std_msgs/MultiArrayDimension
    string label   # label of given dimension
    uint32 size    # size of given dimension (in type units)
    uint32 stride  # stride of given dimension
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new IMG(null);
    if (msg.data !== undefined) {
      resolved.data = std_msgs.msg.UInt8MultiArray.Resolve(msg.data)
    }
    else {
      resolved.data = new std_msgs.msg.UInt8MultiArray()
    }

    if (msg.gtJson_filepath !== undefined) {
      resolved.gtJson_filepath = msg.gtJson_filepath;
    }
    else {
      resolved.gtJson_filepath = ''
    }

    if (msg.msglabel !== undefined) {
      resolved.msglabel = msg.msglabel;
    }
    else {
      resolved.msglabel = ''
    }

    if (msg.sceneid !== undefined) {
      resolved.sceneid = msg.sceneid;
    }
    else {
      resolved.sceneid = 0
    }

    if (msg.imgid !== undefined) {
      resolved.imgid = msg.imgid;
    }
    else {
      resolved.imgid = 0
    }

    return resolved;
    }
};

module.exports = IMG;
