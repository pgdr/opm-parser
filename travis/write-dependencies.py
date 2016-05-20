import requests
import sys

# this be the topological ordering of the projects
projects = ['opm-common','opm-parser','opm-output','opm-grid','opm-material','opm-core','opm-simulators', 'opm-upscaling']
# TODO add ewoms?

base = 'git@github.com:OPM'

deps = {}

for line in sys.stdin:
    if len(line) <= 3 or len(line.split()) != 2:
        continue
    repo, pr = line.split()
    print repo,"\t",pr
    deps[repo] = pr

for proj in projects:
    if proj in deps:
        print 'git clone %s/%s.git' % (base,proj) # specific PR
        print 'cd %s' % proj
        print 'git fetch origin +refs/pull/%s/merge:' % pr
        print 'cd ..'
    else:
        print 'git clone %s/%s.git' % (base,proj) # only master
