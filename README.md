# PetitPaint
C++ University project for image editing

Command list:

    cls -> clears command prompt
    
    toggle -l (x) -> toggles layer (x)
    
    toggle -s (x) -> toggles selection (x)
    
    toggle -s -l (label) -> toggles selection by label
    
    mksel (name) -n (n) -> creates new selection with label (name) containing (n) rects
    
    pwsl -> toggles visibility of all selections (but does not deactivate, use toggle -s (x))
    
    op (name) -> applies operation
    
    op (name) (param) -> applies operation with parameter
    
    op -c (name) -n (n) [-k] -> creates and applies new composite operation composed of (n) operations (if -k is used, it is saved for future use and export)
    
    op -e (name) -f (file_path) -> exports composite operation to a .fun file
    
    op -i (file_path) -> imports composite operation from .fun file
    
    sw -l (x_first) (x_second) -> swaps positions of two layers

    setop (percent) -l (x)  -> set opacity of layer (x)