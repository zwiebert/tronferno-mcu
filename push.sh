tag=$(git describe --tags)
make host-test-all && git push hs && git push origin && git push hs $tag  && git push origin $tag
