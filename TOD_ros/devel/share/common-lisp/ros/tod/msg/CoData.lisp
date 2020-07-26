; Auto-generated. Do not edit!


(cl:in-package tod-msg)


;//! \htmlinclude CoData.msg.html

(cl:defclass <CoData> (roslisp-msg-protocol:ros-message)
  ((data
    :reader data
    :initarg :data
    :type std_msgs-msg:Float32MultiArray
    :initform (cl:make-instance 'std_msgs-msg:Float32MultiArray))
   (msglabel
    :reader msglabel
    :initarg :msglabel
    :type cl:string
    :initform "")
   (sceneid
    :reader sceneid
    :initarg :sceneid
    :type cl:fixnum
    :initform 0)
   (imgid
    :reader imgid
    :initarg :imgid
    :type cl:fixnum
    :initform 0))
)

(cl:defclass CoData (<CoData>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <CoData>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'CoData)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name tod-msg:<CoData> is deprecated: use tod-msg:CoData instead.")))

(cl:ensure-generic-function 'data-val :lambda-list '(m))
(cl:defmethod data-val ((m <CoData>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader tod-msg:data-val is deprecated.  Use tod-msg:data instead.")
  (data m))

(cl:ensure-generic-function 'msglabel-val :lambda-list '(m))
(cl:defmethod msglabel-val ((m <CoData>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader tod-msg:msglabel-val is deprecated.  Use tod-msg:msglabel instead.")
  (msglabel m))

(cl:ensure-generic-function 'sceneid-val :lambda-list '(m))
(cl:defmethod sceneid-val ((m <CoData>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader tod-msg:sceneid-val is deprecated.  Use tod-msg:sceneid instead.")
  (sceneid m))

(cl:ensure-generic-function 'imgid-val :lambda-list '(m))
(cl:defmethod imgid-val ((m <CoData>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader tod-msg:imgid-val is deprecated.  Use tod-msg:imgid instead.")
  (imgid m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <CoData>) ostream)
  "Serializes a message object of type '<CoData>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'data) ostream)
  (cl:let ((__ros_str_len (cl:length (cl:slot-value msg 'msglabel))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_str_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_str_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_str_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_str_len) ostream))
  (cl:map cl:nil #'(cl:lambda (c) (cl:write-byte (cl:char-code c) ostream)) (cl:slot-value msg 'msglabel))
  (cl:let* ((signed (cl:slot-value msg 'sceneid)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 65536) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    )
  (cl:let* ((signed (cl:slot-value msg 'imgid)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 65536) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    )
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <CoData>) istream)
  "Deserializes a message object of type '<CoData>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'data) istream)
    (cl:let ((__ros_str_len 0))
      (cl:setf (cl:ldb (cl:byte 8 0) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'msglabel) (cl:make-string __ros_str_len))
      (cl:dotimes (__ros_str_idx __ros_str_len msg)
        (cl:setf (cl:char (cl:slot-value msg 'msglabel) __ros_str_idx) (cl:code-char (cl:read-byte istream)))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'sceneid) (cl:if (cl:< unsigned 32768) unsigned (cl:- unsigned 65536))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'imgid) (cl:if (cl:< unsigned 32768) unsigned (cl:- unsigned 65536))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<CoData>)))
  "Returns string type for a message object of type '<CoData>"
  "tod/CoData")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'CoData)))
  "Returns string type for a message object of type 'CoData"
  "tod/CoData")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<CoData>)))
  "Returns md5sum for a message object of type '<CoData>"
  "ceef3e91fd77e2286be70524a958bbe2")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'CoData)))
  "Returns md5sum for a message object of type 'CoData"
  "ceef3e91fd77e2286be70524a958bbe2")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<CoData>)))
  "Returns full string definition for message of type '<CoData>"
  (cl:format cl:nil "# Data Structure for Saving Coproducts~%~%std_msgs/Float32MultiArray data~%string msglabel~%int16 sceneid~%int16 imgid~%================================================================================~%MSG: std_msgs/Float32MultiArray~%# Please look at the MultiArrayLayout message definition for~%# documentation on all multiarrays.~%~%MultiArrayLayout  layout        # specification of data layout~%float32[]         data          # array of data~%~%~%================================================================================~%MSG: std_msgs/MultiArrayLayout~%# The multiarray declares a generic multi-dimensional array of a~%# particular data type.  Dimensions are ordered from outer most~%# to inner most.~%~%MultiArrayDimension[] dim # Array of dimension properties~%uint32 data_offset        # padding elements at front of data~%~%# Accessors should ALWAYS be written in terms of dimension stride~%# and specified outer-most dimension first.~%# ~%# multiarray(i,j,k) = data[data_offset + dim_stride[1]*i + dim_stride[2]*j + k]~%#~%# A standard, 3-channel 640x480 image with interleaved color channels~%# would be specified as:~%#~%# dim[0].label  = \"height\"~%# dim[0].size   = 480~%# dim[0].stride = 3*640*480 = 921600  (note dim[0] stride is just size of image)~%# dim[1].label  = \"width\"~%# dim[1].size   = 640~%# dim[1].stride = 3*640 = 1920~%# dim[2].label  = \"channel\"~%# dim[2].size   = 3~%# dim[2].stride = 3~%#~%# multiarray(i,j,k) refers to the ith row, jth column, and kth channel.~%~%================================================================================~%MSG: std_msgs/MultiArrayDimension~%string label   # label of given dimension~%uint32 size    # size of given dimension (in type units)~%uint32 stride  # stride of given dimension~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'CoData)))
  "Returns full string definition for message of type 'CoData"
  (cl:format cl:nil "# Data Structure for Saving Coproducts~%~%std_msgs/Float32MultiArray data~%string msglabel~%int16 sceneid~%int16 imgid~%================================================================================~%MSG: std_msgs/Float32MultiArray~%# Please look at the MultiArrayLayout message definition for~%# documentation on all multiarrays.~%~%MultiArrayLayout  layout        # specification of data layout~%float32[]         data          # array of data~%~%~%================================================================================~%MSG: std_msgs/MultiArrayLayout~%# The multiarray declares a generic multi-dimensional array of a~%# particular data type.  Dimensions are ordered from outer most~%# to inner most.~%~%MultiArrayDimension[] dim # Array of dimension properties~%uint32 data_offset        # padding elements at front of data~%~%# Accessors should ALWAYS be written in terms of dimension stride~%# and specified outer-most dimension first.~%# ~%# multiarray(i,j,k) = data[data_offset + dim_stride[1]*i + dim_stride[2]*j + k]~%#~%# A standard, 3-channel 640x480 image with interleaved color channels~%# would be specified as:~%#~%# dim[0].label  = \"height\"~%# dim[0].size   = 480~%# dim[0].stride = 3*640*480 = 921600  (note dim[0] stride is just size of image)~%# dim[1].label  = \"width\"~%# dim[1].size   = 640~%# dim[1].stride = 3*640 = 1920~%# dim[2].label  = \"channel\"~%# dim[2].size   = 3~%# dim[2].stride = 3~%#~%# multiarray(i,j,k) refers to the ith row, jth column, and kth channel.~%~%================================================================================~%MSG: std_msgs/MultiArrayDimension~%string label   # label of given dimension~%uint32 size    # size of given dimension (in type units)~%uint32 stride  # stride of given dimension~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <CoData>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'data))
     4 (cl:length (cl:slot-value msg 'msglabel))
     2
     2
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <CoData>))
  "Converts a ROS message object to a list"
  (cl:list 'CoData
    (cl:cons ':data (data msg))
    (cl:cons ':msglabel (msglabel msg))
    (cl:cons ':sceneid (sceneid msg))
    (cl:cons ':imgid (imgid msg))
))
