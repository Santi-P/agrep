
for i in {1.. 10}; do
shuf -n 10 tests/search_words.txt | xargs -I{}  ./bin/agrep {}  tests/bible.txt;
done

for i in {1.. 10}; do
shuf -n 10 tests/search_words.txt | xargs -I{}  ./bin/agrep -k1 {}  tests/bible.txt;
done

for i in {1.. 10}; do
shuf -n 10 tests/search_words.txt | xargs -I{}  ./bin/agrep -k2 {}  tests/bible.txt;
done

for i in {1.. 10}; do
shuf -n 10 tests/search_words.txt | xargs -I{}  ./bin/agrep -e {}   tests/bible.txt;
done

for i in {1.. 10}; do
shuf -n 10 tests/search_words.txt | xargs -I{}  ./bin/agrep -e -k1 {}  tests/bible.txt;
done

for i in {1.. 10}; do
shuf -n 10 tests/search_words.txt | xargs -I{}  ./bin/agrep  -e -k2 {}  tests/bible.txt;
done

shuf -n 100 tests/search_words.txt > tmp_search.txt;
cat tmp_search.txt | xargs -I{}  ./bin/agrep  {}  tests/bible.txt > my_results.txt;
cat tmp_search.txt | xargs -I{}  grep -n  {}  tests/bible.txt > grep_results.txt;
python tests/verify_res.py my_results.txt grep_results.txt
