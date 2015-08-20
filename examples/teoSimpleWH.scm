;; ---------------------------------------------------------------------
;;  GeoViS:  teoSimpleWH.scm
;;
;;  Author:  Thomas Mueller, University of Stuttgart, VISUS
;;           2015-08-20
;;
;;    Shadow of a simple Teo wormhole.
;;
;;    Wormhole throat: b0 = 1
;;
;;    Observer
;;        radial position        r_obs = 15rs
;;        
;;    You need a 4pi background texture, e.g. the Milky Way panorama
;;    by ESO/S.Brunier:
;;    http://www.eso.org/public/germany/images/eso0932a/    
;; ---------------------------------------------------------------------

(define homedir (getenv "HOME"))
;(define background_upper (string-append homedir "/local/Texturen/MilkyWay/eso0932a2.png"))
(define background_upper (string-append homedir "/local/Texturen/Panorama/Interstellar/InterstellarWormhole_Fig10.png"))
(define background_lower (string-append homedir "/local/Texturen/Panorama/Interstellar/InterstellarWormhole_Fig6b.png"))
;(define background_image_name "examples/eso0932a.tif")

(define r_obs 3.0 )
(define fov   20.0 )

;; --- Initialize spacetime metric
(init-metric '(type "TeoSimpleWH")
             '(b0 1.0)
             '(id "metric")
)


;; --- Initialize integrator for light rays
(init-solver '(type     "GSL_RK_Cash-Karp")
             '(geodType "lightlike")
             '(eps_abs  1.0e-8)
             '(step_size 0.01)
             '(step_ctrl #t)
             '(id "raytracing")
)

;; --- Initialize pinhole camera
(init-camera '(type "PinHoleCam")
             `(dir #( 1.0 0.0 0.0) )
             '(vup #( 0.0 0.0 1.0) )
             `(fov ,(vector fov fov ))
             '(res #(150 150))
             '(filter "FilterRGB")
             '(id "cam")
)

;; --- Initialize 4pi camera
(init-camera '(type "4PICam")
             '(angle  0.0)
             '(res #( 360 180 ))
             ;'(res #( 1600 800 ))
             '(filter "FilterRGB")
             '(id "skycam")
)

;; --- Initialize ray generator
(init-raygen '(type       "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX)  -70.0 (- gpDBLMAX) (- gpDBLMAX)) )
             `(boundBoxUR  ,(vector   gpDBLMAX    70.0  gpDBLMAX gpDBLMAX) )
             '(solver "raytracing")
             '(maxNumPoints 5000)
)

;; --- Set local reference frame of observer
(local-tetrad `(pos ,(vector 0.0 r_obs 1.5707963 0.0 ))
              '(e0  #(1.0  0  0  0) )
              '(e1  #(0.0 -1  0  0) )
              '(e2  #(0.0  0  0 -1) )
              '(e3  #(0.0  0 -1  0) )
              '(incoords #f) 
              '(id  "locTedObs")
)

;; --- Initialize integrator for timelike geodesic
(init-solver '(type     "GSL_RK_Fehlberg")
             '(geodType "timelike")
             '(step_ctrl #f)
             '(step_size 0.01)
             '(id "gsolver")
)

;; --- Calculate timelike geodesic for star motion
(init-motion '(type "Geodesic")
             '(solver "gsolver")
             `(pos ,(vector 0.0  r_obs PIhalf 0.0 ))
             `(localvel ,(vector -0.05 0.0 0.0))
             '(e0 #( 1.0  0.0  0.0  0.0 ))
             '(e1 #( 0.0 -1.0  0.0  0.0 ))
             '(e2 #( 0.0  0.0  0.0 -1.0 ))
             '(e3 #( 0.0  0.0 -1.0  0.0 ))
             '(maxnumpoints 20000)
             '(forward  200.0)
             '(backward 0.0)
             '(id "obsMotion")
)

;(gvs-print-motion '(id "obsMotion")
;          '(file "obsMotion.dat")
;)


;; --- Initialize projector with observer tetrad
;(init-projector '(localTetrad "locTedObs")
(init-projector '(motion "obsMotion")
                '(color #(0.0 0.0 0.0))
                '(id "proj")
)

;; --- Set ambient light
(init-light-mgr '(ambient #(1.0 1.0 1.0)))


;; --- Set image texture for the background
(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "Image")
                                        `(file ,background_upper)
                                        `(transform ,(scale-obj #(-1.0 -1.0)))
                                        )
                         )
              '(ambient 1.0)
              '(id "bgShaderUpper")
)

;; --- Set sphere as representative for the background
(solid-ellipsoid `(objtype ,gpObjTypeInCoords)
                 '(center #(0.0 0.0 0.0))
                 '(axlen  #(60.0 60.0 60.0))
                 `(transform ,(rotate-obj "z" 0.0))
                 '(shader "bgShaderUpper")
                 '(chart 0)
                 '(id "bgimageUpper")
)

;; --- Set image texture for the background
(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "Image")
                                        `(file ,background_lower)
                                        `(transform ,(scale-obj #(-1.0 -1.0)))
                                        )
                         )
              '(ambient 1.0)
              '(id "bgShaderLower")
)

;; --- Set sphere as representative for the background
(solid-ellipsoid `(objtype ,gpObjTypeInCoords)
                 '(center #(0.0 0.0 0.0))
                 '(axlen  #(60.0 60.0 60.0))
                 `(transform ,(rotate-obj "z" 0.0))
                 '(shader "bgShaderLower")
                 '(chart 1)
                 '(id "bgimageLower")
)

(comp-object '(obj "bgimageUpper")
             '(obj "bgimageLower")
             '(id "scene")
)

(do ((count 0 (+ count 1))) ((= count 2000))
    (init-device '(type "standard")
                 '(obj "scene")
                 ;'(camera "cam")
                 '(camera "skycam")
                 `(setparam ("proj" "actualpos" ,(* 10 count)))
    )
)
