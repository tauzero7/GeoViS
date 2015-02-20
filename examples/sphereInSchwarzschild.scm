;; ---------------------------------------------------------------------
;;  GeoViS:  sphereInSchwarzschild.scm
;;
;;  Author:  Thomas Mueller, University of Stuttgart, VISUS
;;           2015-01-03
;;
;;    A checkered sphere located in the Schwarzschild metric
;;    
;; ---------------------------------------------------------------------

(define r_obs  30.0 )
(define azi    -1.5707963 )
(define fview  5.0 )
;(define fview 2.5)

(define r_obj  6.0 )

(define vDir  13.5)

;; --- Initialize spacetime metric
(init-metric '(type "Schwarzschild")
             '(mass 1.0)
             '(id "metric")
)

;; --- Initialize integrator for light rays
(init-solver '(type     "GSL_RK_Cash-Karp")
             '(geodType "lightlike")
             '(eps_abs  1.0e-8)
             '(step_size 0.01)
             '(id "raytracing")
)

;; --- Initialize observer camera
(init-camera '(type "PinHoleCam")
             `(dir ,(vector (sin (* vDir DEG_TO_RAD)) (cos (* vDir DEG_TO_RAD)) 0.0) )
             '(vup #( 0.0 0.0 1.0) )
             `(fov ,(vector  fview fview ))
             '(res #(100 100))
             '(filter "FilterRGB")
             '(id "cam1")
)

;; --- Initialize ray generator
(init-raygen '(type       "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX)  0.0 (- gpDBLMAX) (- gpDBLMAX)) )
             `(boundBoxUR  ,(vector   gpDBLMAX   50.0    gpDBLMAX     gpDBLMAX ) )
             '(solver "raytracing")
             '(maxNumPoints 2000)
)

;; --- Set local reference frame of observer
(local-tetrad `(pos ,(vector  0.0 r_obs PIhalf azi ))
              '(e0  #(1.0  0.0  0.0  0.0) )
              '(e1  #(0.0  0.0  0.0  1.0) )
              '(e2  #(0.0 -1.0  0.0  0.0) )
              '(e3  #(0.0  0.0 -1.0  0.0) )
              '(incoords #f) 
              '(id  "locTedObs")
)

;; --- Initialize projector with observer tetrad
(init-projector '(localTetrad "locTedObs")
                '(color #(0.1 0.1 0.1))
                '(id "proj")
)

;; --- Set ambient light
(init-light-mgr '(ambient #(1.0 1.0 1.0)) )


;; --- Set uniform texture 1 for sphere shading
(init-texture '(type "UniTex")
              '(color #(0.8 0.16 0.16))
              '(id "utex1")
)

;; --- Set uniform texture 2 for sphere shading
(init-texture '(type "UniTex")
              '(color #(0.9 0.63 0.63))
              '(id "utex2")
)

;; --- Set surface shader for sphere as checkerboard texture
(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "CheckerT2D")
                         '(texture "utex1")
                         '(texture "utex2")
                         `(transform ,(scale-obj #(20.0 10.0)))
                         )
               )
              '(ambient 0.2)
              '(diffuse 1.0)
              '(id "sphereShader")
)

;; --- Set sphere as representative for object
(solid-ellipsoid `(objtype ,gpObjTypeLocal)
                 '(center #(0.0 0.0 0.0))
                 '(axlen  #(0.5 0.5 0.5))
                 '(shader "sphereShader")
                 '(id "sphere")
)


(solid-ellipsoid `(objtype ,gpObjTypeInCoords)
                 '(center #(0.0 0.0 0.0))
                 '(axlen  #(0.5 0.5 0.5))
                 '(shader "sphereShader")
                 `(transform ,(translate-obj #(6.0 0.0 0.0) (rotate-obj "y" 90.0)))
                 '(id "coordSphere")
)

(mesh-obj `(objtype ,gpObjTypeInCoords)
          '(filename "sl_sphere.obj")
          ;'(filename "ll_sphere.obj")
          '(pathname "/home/tmueller/develop/M4Dprogs/CoordSphere")          
          '(shader "sphereShader")          
          '(id "meshSphere")
)


;; --- Set a static tetrad as alternative for sphere motion
(local-tetrad `(pos ,(vector 0.0 r_obj PIhalf 0.0))
              '(e0 #(1.0 0.0 0.0 0.0))
              '(e1 #(0.0 0.0 -1.0 0.0))
              '(e2 #(0.0 0.0 0.0 -1.0))
              '(e3 #(0.0 1.0 0.0 0.0))
              '(incoords #f) 
              '(id  "locTedSphere")
)

;; --- Combine sphere object and motion into one local object
(local-comp-object '(obj "sphere")
                   '(localtetrad "locTedSphere")
                   '(id "lco1")
)

;; --- Generate image sequence
(init-device '(type "standard")
             ;'(obj "lco1")             
             ;'(obj "coordSphere")
             '(obj "meshSphere")
)

