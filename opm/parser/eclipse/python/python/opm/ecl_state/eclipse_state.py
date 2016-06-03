import os.path

from ert.cwrap import BaseCClass
from opm import OPMPrototype
from opm import IntVector, DoubleVector

class EclipseState(BaseCClass):
    TYPE_NAME   = "eclipse_state"
    _parse               = OPMPrototype("eclipse_state_obj     eclipse_state_parse( char* )", bind = False)
    _has_int_keyword     = OPMPrototype("bool                  eclipse_state_has_int_keyword( eclipse_state , char* )")
    _has_double_keyword  = OPMPrototype("bool                  eclipse_state_has_double_keyword( eclipse_state , char* )")
    _get_double_keyword  = OPMPrototype("opm_double_vector_ref eclipse_state_get_double_keyword( eclipse_state , char* )")
    _get_int_keyword     = OPMPrototype("opm_int_vector_ref    eclipse_state_get_int_keyword( eclipse_state , char* )")
    _get_title           = OPMPrototype("char*                 eclipse_state_get_title( eclipse_state )")

    def supportedKeywords(self):
        return None

    def __init__(self , *args, **kwargs):
        raise NotImplementedError("Can not instantiate empty state, use parse")

    def __contains__(self , keyword):
        return self._has_int_keyword( keyword ) or self._has_double_keyword( keyword )

    @staticmethod
    def parseDeck(filename):
        print 'attempting to parse deck at %s ...' % filename
        return EclipseState._parse(filename)

    def __getitem__(self, key):
        if not key in self:
            raise KeyError("Keyword %s not in deck" % key)
        if self._has_double_keyword(key):
            return [x for x in self._get_double_keyword(key)]
        elif self._has_int_keyword(key):
            return [x for x in self._get_int_keyword(key)]
        raise ValueError("Unknown error for key %s" % key)

    def title(self):
        return self._get_title()

    def __str__(self):
        return 'EclipseState: ' + self.title()

    def free(self):
        # self._free()
        pass
