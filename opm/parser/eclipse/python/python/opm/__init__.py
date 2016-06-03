import ctypes
from ert.cwrap.clib import lib_name
from ert.cwrap.metacwrap import Prototype

        
lib_path = None
so_version = ""

try:
    import __opm_lib_info
    lib_path = __opm_lib_info.lib_path
    so_version = __opm_lib_info.so_version
except ImportError:
    # pass
    lib_path = "/private/pgdr/opm/opm-parser/build/lib64"
    so_version = ""


try:
    import opm_site_init
except ImportError:
    pass



class OPMPrototype(Prototype):
    lib_file = lib_name( "libcopmparser" , path = lib_path , so_version = so_version)
    lib = ctypes.CDLL( lib_file , ctypes.RTLD_GLOBAL )

    def __init__(self , prototype , bind = True):
        super(OPMPrototype , self).__init__( OPMPrototype.lib , prototype , bind = bind)

from .int_vector import IntVector
from .double_vector import DoubleVector
