;; Auto-generated. Do not edit!


(when (boundp 'tod::ReqImage)
  (if (not (find-package "TOD"))
    (make-package "TOD"))
  (shadow 'ReqImage (find-package "TOD")))
(unless (find-package "TOD::REQIMAGE")
  (make-package "TOD::REQIMAGE"))
(unless (find-package "TOD::REQIMAGEREQUEST")
  (make-package "TOD::REQIMAGEREQUEST"))
(unless (find-package "TOD::REQIMAGERESPONSE")
  (make-package "TOD::REQIMAGERESPONSE"))

(in-package "ROS")





(defclass tod::ReqImageRequest
  :super ros::object
  :slots (_req ))

(defmethod tod::ReqImageRequest
  (:init
   (&key
    ((:req __req) nil)
    )
   (send-super :init)
   (setq _req __req)
   self)
  (:req
   (&optional __req)
   (if __req (setq _req __req)) _req)
  (:serialization-length
   ()
   (+
    ;; bool _req
    1
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; bool _req
       (if _req (write-byte -1 s) (write-byte 0 s))
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; bool _req
     (setq _req (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;;
   self)
  )

(defclass tod::ReqImageResponse
  :super ros::object
  :slots (_ok ))

(defmethod tod::ReqImageResponse
  (:init
   (&key
    ((:ok __ok) nil)
    )
   (send-super :init)
   (setq _ok __ok)
   self)
  (:ok
   (&optional __ok)
   (if __ok (setq _ok __ok)) _ok)
  (:serialization-length
   ()
   (+
    ;; bool _ok
    1
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; bool _ok
       (if _ok (write-byte -1 s) (write-byte 0 s))
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; bool _ok
     (setq _ok (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;;
   self)
  )

(defclass tod::ReqImage
  :super ros::object
  :slots ())

(setf (get tod::ReqImage :md5sum-) "6abf24ca3eea6d1d556c12b7504c3b47")
(setf (get tod::ReqImage :datatype-) "tod/ReqImage")
(setf (get tod::ReqImage :request) tod::ReqImageRequest)
(setf (get tod::ReqImage :response) tod::ReqImageResponse)

(defmethod tod::ReqImageRequest
  (:response () (instance tod::ReqImageResponse :init)))

(setf (get tod::ReqImageRequest :md5sum-) "6abf24ca3eea6d1d556c12b7504c3b47")
(setf (get tod::ReqImageRequest :datatype-) "tod/ReqImageRequest")
(setf (get tod::ReqImageRequest :definition-)
      "bool req
---
bool ok
")

(setf (get tod::ReqImageResponse :md5sum-) "6abf24ca3eea6d1d556c12b7504c3b47")
(setf (get tod::ReqImageResponse :datatype-) "tod/ReqImageResponse")
(setf (get tod::ReqImageResponse :definition-)
      "bool req
---
bool ok
")



(provide :tod/ReqImage "6abf24ca3eea6d1d556c12b7504c3b47")


