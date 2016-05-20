import requests
import sys
from re import finditer # oh no! big mistake

# this be the topological ordering of the projects
projects = ['opm-common','opm-parser','opm-output','opm-grid','opm-material','opm-core','opm-simulators', 'opm-upscaling']
# TODO add ewoms?

repos = []

for x in projects:
    repos.append('OPM\\/' + x)


def get_pull_url(project, pr):
    return 'https://api.github.com/repos/OPM/%s/pulls/%s' % (project, pr)

def get_pr_json(project, pr):
    x_url = get_pull_url(project, pr)
    x_r = requests.get(x_url)
    if (x_r.status_code != 200):
        if (400 <= x_r.status_code <= 499):
            print 'Error: ', x_r.status_code
            try:
                warn_msg = x_r.json()['message']
                print 'Warning: ' + warn_msg
            except Exception:
                pass
        exit(1)
    x_json = x_r.json()
    if (len(x_json)<=2):
        if 'message' in x_json:
            print 'Warning: ' + x_json['message']
        exit(1)
    return x_json

def get_mentioned_pulls(msg):
    """
    Gets a list of strings on the following form:
    ['OPM/opm-parser#806', 'OPM/opm-simulators#680', 'OPM/opm-output#27']
    """
    global repos
    # regex is (r1|r2|r3)\#\d+
    regexp = '(' + '|'.join(repos)
    regexp += ')\\#\\d+'
    starts = [m.start() for m in finditer(regexp, msg)]
    strings = []
    for i in starts:
        id_i = msg.find('#',i)
        for j in range(id_i+1,len(msg)):
            if not msg[j].isdigit():
                break
        if j == len(msg) - 1 and msg[j].isdigit():
            j += 1
        strings.append(msg[i:j])
    return strings

 # usage: python find-dependencies.py repo pr
if len(sys.argv) != 3:
    print 'Usage:    python find-dependencies.py repo PR'
    print 'Example:  python find-dependencies.py opm-parser 806'
    exit(1)

this_project = sys.argv[1]
this_pr = sys.argv[2]
if this_project not in projects:
    print 'Not supported project: ' + this_project
    print '\nSupported projects:'
    for x in projects:
        print '\t' + x


pulls_json = get_pr_json(this_project, this_pr)
if len(pulls_json) == 0:
    print 'No such PR: %s\#%s' %(this_project,this_pr)
    print 'Debug, try this url: %s' % get_pull_url(this_project,this_pr)
    exit(1)

if 'state' not in pulls_json or pulls_json['state'] != 'open':
    print 'Warning: PR not open!'
    if 'state' in pulls_json:
        print '         ' + pulls_json['state']

pulls_body = str(pulls_json['body'].decode('utf8'))

prs = get_mentioned_pulls(pulls_body)

for x in prs:
    x_repo, x_pr = x.split('#')
    x_repo = x_repo[4:] # remove OPM/
    x_json = get_pr_json(x_repo, x_pr)
    if (len(x_json) == 0):
        continue
    if x_json['state'] == 'open':
        print '%s %s' % (x_repo, x_pr)
    #else:
    #    print '# ignoring %s PR %s#%s'% (x_json['state'], x_repo, x_pr)
