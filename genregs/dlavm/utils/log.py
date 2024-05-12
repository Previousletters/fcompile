_logs = {}


def LOG_WITH_PREFIX(prefix, log):
    global _logs
    if prefix not in _logs:
        _logs[prefix] = []
    _logs[prefix].append(log)


def LOG_CLEAR():
    global _logs
    _logs = {}


def GET_LOG():
    return _logs