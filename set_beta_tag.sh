make host-test-all && \
git branch -D beta; git branch beta && git tag -f -a "$1" -m "" 
