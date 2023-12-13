ACCEL_DRIVER = {}


def register_driver(accel, op, version):

    def _register_fn(fn):
        global ACCEL_DRIVER
        if accel not in ACCEL_DRIVER:
            ACCEL_DRIVER[accel] = {}
        if op not in ACCEL_DRIVER[accel]:
            ACCEL_DRIVER[accel][op] = {}
        ACCEL_DRIVER[accel][op][version] = fn

        def _call(*args):
            return fn(*args)
        return _call
    return _register_fn


def get_driver(accel, op, version="last"):
    if accel not in ACCEL_DRIVER:
        raise RuntimeError("No " + accel + " driver found, please register first!")
    if op not in ACCEL_DRIVER[accel]:
        raise RuntimeError("No " + accel + "." + op + " driver found, please register first!")
    if version == "last":
        version = list(ACCEL_DRIVER[accel][op].keys())[-1]
        return ACCEL_DRIVER[accel][op][version]
    elif version in ACCEL_DRIVER[accel][op]:
        return ACCEL_DRIVER[accel][op][version]
    else:
        raise RuntimeError("No " + accel + "." + op + "." + version + " driver found, please register first!")
