;; Auto-generated. Do not edit!


(when (boundp 'tod::StartEval)
  (if (not (find-package "TOD"))
    (make-package "TOD"))
  (shadow 'StartEval (find-package "TOD")))
(unless (find-package "TOD::STARTEVAL")
  (make-package "TOD::STARTEVAL"))
(unless (find-package "TOD::STARTEVALREQUEST")
  (make-package "TOD::STARTEVALREQUEST"))
(unless (find-package "TOD::STARTEVALRESPONSE")
  (make-package "TOD::STARTEVALRESPONSE"))

(in-package "ROS")





(defclass tod::StartEvalRequest
  :super ros::object
  :slots (_start ))

(defmethod tod::StartEvalRequest
  (:init
   (&key
    ((:start __start) nil)
    )
   (send-super :init)
   (setq _start __start)
   self)
  (:start
   (&optional __start)
   (if __start (setq _start __start)) _start)
  (:serialization-length
   ()
   (+
    ;; bool _start
    1
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; bool _start
       (if _start (write-byte -1 s) (write-byte 0 s))
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; bool _start
     (setq _start (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;;
   self)
  )

(defclass tod::StartEvalResponse
  :super ros::object
  :slots (_ok ))

(defmethod tod::StartEvalResponse
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

(defclass tod::StartEval
  :super ros::object
  :slots ())

(setf (get tod::StartEval :md5sum-) "93d7245f95bf423437148710b6b287c7")
(setf (get tod::StartEval :datatype-) "tod/StartEval")
(setf (get tod::StartEval :request) tod::StartEvalRequest)
(setf (get tod::StartEval :response) tod::StartEvalResponse)

(defmethod tod::StartEvalRequest
  (:response () (instance tod::StartEvalResponse :init)))

(setf (get tod::StartEvalRequest :md5sum-) "93d7245f95bf423437148710b6b287c7")
(setf (get tod::StartEvalRequest :datatype-) "tod/StartEvalRequest")
(setf (get tod::StartEvalRequest :definition-)
      "bool start
---
bool ok
")

(setf (get tod::StartEvalResponse :md5sum-) "93d7245f95bf423437148710b6b287c7")
(setf (get tod::StartEvalResponse :datatype-) "tod/StartEvalResponse")
(setf (get tod::StartEvalResponse :definition-)
      "bool start
---
bool ok
")



(provide :tod/StartEval "93d7245f95bf423437148710b6b287c7")


