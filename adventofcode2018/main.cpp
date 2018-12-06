﻿#include "harness.h"

#include <algorithm>
#include <ctime>
#include <map>
#include <numeric>
#include <set>
#include <unordered_set>
#include <vector>


#define READ(str)       stringlist::fromstring(str)
#define LOAD(day)       stringlist::fromfile("day" #day ".txt")
#define LOADSTR(day)    stringfromfile("day" #day ".txt")


// -------------------------------------------------------------------

int day1(const stringlist& lines)
{
    int acc = 0;
    for (auto line : lines)
    {
        int iline = stoi(line);
        acc += iline;
    }
    return acc;
}

int day1_2(const stringlist& lines)
{
    const int max_accum = 1000000;
    vector<uint8_t> seen(2 * max_accum, 0);

    int acc = 0;
    seen[max_accum] = 1;

    for(;;)
    {
        for (auto line : lines)
        {
            int iline = stoi(line);
            acc += iline;
            _ASSERT(acc < max_accum);
            _ASSERT(-acc < max_accum);

            if (seen[acc + max_accum])
                return acc;

            seen[acc + max_accum] = 1;
        }
    }
    return -11111111;
}


// -------------------------------------------------------------------

int day2(const stringlist& lines)
{
    int numdoubles = 0;
    int numtriples = 0;

    for (auto line : lines)
    {
        const int numletters = 26;
        int seencount[numletters];
        memset(seencount, 0, sizeof(seencount));

        for (auto c : line)
        {
            int ix = c - 'a';
            seencount[ix]++;
        }

        int dub = 0;
        int trip = 0;
        for (int c = 0; c < numletters; ++c)
        {
            if (seencount[c] == 2)
                dub = 1;
            else if (seencount[c] == 3)
                trip = 1;

            if (dub + trip == 2)
                break;
        }
        numdoubles += dub;
        numtriples += trip;
    }

    return numdoubles * numtriples;
}

// returns the index of characters that are mismatched, or <0 if strings don't differ by a single char
int d2_compare(const string& a, const string& b)
{
    int diffpos = -1;

    auto ita = a.begin();
    auto itb = b.begin();
    for (; ita != a.end(); ++ita, ++itb)
    {
        if (*ita != *itb)
        {
            // >1 difference
            if (diffpos >= 0)
                return -1;

            diffpos = distance(a.begin(), ita);
        }
    }

    return diffpos;
}

string day2_2(const stringlist& lines)
{
    for (auto itline = lines.begin(); itline != lines.end(); ++itline)
    {
        const string& teststr = *itline;
        for (auto ittest = lines.begin(); ittest != itline; ++ittest)
        {
            int diff = d2_compare(teststr, *ittest);
            if (diff < 0)
                continue;

            // found it!!
            {
                string common = teststr.substr(0, diff);
                common += teststr.substr(diff + 1);
                return common;
            }
        }
    }

    return "oh dear";
}

// -------------------------------------------------------------------

const int d3_clothsz = 1000;

int32_t d3_mark(int16_t* pcloth, int claim, int left, int top, int w, int h, bool* clean)
{
    _ASSERT(left + w <= d3_clothsz);
    _ASSERT(top + h <= d3_clothsz);

    int32_t newoverlap = 0;
    bool overlap = false;

    int16_t* pcurr = pcloth + (top * d3_clothsz) + left;
    for (int y = top; y < top + h; ++y)
    {
        for (int x = left; x < left + w; ++x, ++pcurr)
        {
            auto curr = *pcurr;
            if (curr == 0 || curr == claim)
            {
                *pcurr = claim;
            }
            else if (curr > 0)
            {
                newoverlap++;
                *pcurr = -1;
                overlap = true;
            }
            else
                overlap = true;
        }

        pcurr += d3_clothsz - w;
    }

    if (!overlap && clean)
        *clean = true;

    return newoverlap;
}

int32_t day3(const stringlist& input)
{
    int16_t* pclothbuf = new int16_t[d3_clothsz * d3_clothsz];
    memset(pclothbuf, 0, d3_clothsz * d3_clothsz);

    int32_t overlap = 0;
    for (auto line : input)
    {
        // #1 @ 1,3: 4x4
        istringstream is(line);
        char hash, at, comma, colon, by;
        int claim, left, top, width, height;
        is >> hash >> claim >> ws >> at >> ws >> left >> comma >> top >> colon >> ws >> width >> by >> height;
        overlap += d3_mark(pclothbuf, claim, left, top, width, height, NULL);
    }

    delete[] pclothbuf;
    return overlap;
}

int32_t day3_2(const stringlist& input)
{
    int16_t* pclothbuf = new int16_t[d3_clothsz * d3_clothsz];
    memset(pclothbuf, 0, d3_clothsz * d3_clothsz);

    // fill
    for (auto line : input)
    {
        // #1 @ 1,3: 4x4
        istringstream is(line);
        char hash, at, comma, colon, by;
        int claim, left, top, width, height;
        is >> hash >> claim >> ws >> at >> ws >> left >> comma >> top >> colon >> ws >> width >> by >> height;
        d3_mark(pclothbuf, claim, left, top, width, height, NULL);
    }
    // find clean
    int cleanclaim = -1;
    for (auto line : input)
    {
        // #1 @ 1,3: 4x4
        istringstream is(line);
        char hash, at, comma, colon, by;
        int claim, left, top, width, height;
        is >> hash >> claim >> ws >> at >> ws >> left >> comma >> top >> colon >> ws >> width >> by >> height;
        bool clean = false;
        d3_mark(pclothbuf, claim, left, top, width, height, &clean);

        if (clean)
        {
            cleanclaim = claim;
            break;
        }
    }

    delete[] pclothbuf;
    return cleanclaim;
}

// -------------------------------------------------------------------

struct D4Day
{
    int8_t month;
    int8_t day;
    int16_t guard;
    uint8_t minute[60]; // 1=asleep,0=awake

    D4Day() {/**/}
    D4Day(int m, int d, int g) : month((int8_t)m), day((int8_t)d), guard((int16_t)g)
    {
        memset(minute, 0, sizeof(minute));
    }
};

int d4_find_sleepiest_guard(const vector<D4Day>& days)
{
    // build map of guard -> sleepiness
    map<int, int> guard_sleep;
    for (auto day : days)
    {
        auto itgs = guard_sleep.find(day.guard);
        if (itgs == guard_sleep.end())
            itgs = guard_sleep.insert(make_pair(day.guard, 0)).first;

        itgs->second += accumulate(day.minute, day.minute + 60, 0);
    }

    // find the guard who's asleep the most
    int guard = -1;
    int maxsleep = -1;
    for (auto kv : guard_sleep)
    {
        if (kv.second > maxsleep)
        {
            guard = kv.first;
            maxsleep = kv.second;
        }
    }

    return guard;
}

int d4_find_sleepiest_minute(const vector<D4Day>& days, int guard)
{
    uint8_t minutes[60];
    memset(minutes, 0, sizeof(minutes));

    for (auto day : days)
    {
        if (day.guard != guard)
            continue;

        for (int i = 0; i < 60; ++i)
        {
            minutes[i] += day.minute[i];
        }
    }

    auto itsleepy = max_element(minutes, minutes + 60);

    return (int)distance(minutes, itsleepy);
}


void d4_load(stringlist& input, vector<D4Day>& outdays)
{
    // input not date sorted
    input.sort();

    D4Day* pday = NULL;
    int sleepmin = -1;

    for (auto line : input)
    {
        // [1518-11-01 00:00] Guard #10 begins shift
        // [1518-11-01 00:05] falls asleep
        // [1518-11-01 00:25] wakes up
        istringstream is(line);
        int month, day, hour, minute;
        char dummy;
        is.ignore(6);
        is >> month >> dummy >> day >> ws >> hour >> dummy >> minute >> dummy >> ws;
        string event(std::istreambuf_iterator<char>(is), {});   // read the rest of the buffer into a string. could also just use substr...

        if (event[0] == 'G')
        {
            _ASSERT(sleepmin < 0);
            istringstream isev(event);
            isev.ignore(7);
            int guard;
            isev >> guard;
            outdays.push_back(D4Day(month, day, guard));
            pday = &outdays.back();
        }
        else if (event[0] == 'f')
        {
            _ASSERT(sleepmin < 0);
            sleepmin = minute;
        }
        else
        {
            _ASSERT(event[0] == 'w');
            _ASSERT(sleepmin >= 0);
            _ASSERT(sleepmin < 60);
            _ASSERT(minute <= 60);
            _ASSERT(minute > 0);
            fill(pday->minute + sleepmin, pday->minute + minute, 1);
            sleepmin = -1;
        }
    }
}

int day4(stringlist& input)
{
    vector<D4Day> days;
    d4_load(input, days);

    int sleepy = d4_find_sleepiest_guard(days);
    int sleepiest_minute = d4_find_sleepiest_minute(days, sleepy);

    return sleepy * sleepiest_minute;
}

int day4_2(stringlist& input)
{
    vector<D4Day> days;
    d4_load(input, days);

    map<int, vector<uint8_t> > guard_minutes;
    for (auto& day : days)
    {
        auto itgm = guard_minutes.find(day.guard);
        if (itgm == guard_minutes.end())
        {
            itgm = guard_minutes.emplace(make_pair(day.guard, vector<uint8_t>(60,0))).first;
        }

        for (int i = 0; i < 60; ++i)
            itgm->second[i] += day.minute[i];
    }

    int guard = -1;
    int minute = -1;
    int sleepiness = -1;
    for (auto g_mins : guard_minutes)
    {
        // find the sleepiest minute
        auto itsleepy = max_element(g_mins.second.begin(), g_mins.second.end());
        if (*itsleepy > sleepiness)
        {
            guard = g_mins.first;
            sleepiness = *itsleepy;
            minute = distance(g_mins.second.begin(), itsleepy);
        }
    }

    return guard * minute;
}

// -------------------------------------------------------------------

int day5(string& s)
{
    // backwards for less copying

    // iterator overhead is crippling in debug builds; let's try it without!
    // RESULT: ~2x speedup. remaining time mostly in string.erase
    //   THINKS: maybe do something clever with batching contiguous reactions? doesn't look like it'll be much faster tho...
    char* prbegin = &s.back();
    char* prend = (&s.front() - 1);

    auto itbegin = s.begin();

    char danger = 0;
    for (auto it = prbegin; it != prend; --it)
    {
        if (*it == danger)
        {
            // move back to previous
            ++it;
            // erase it and it's next (this is all bakcwards, so we actually moved fwd 1 then erase from 2 back)
            auto itc = itbegin + (it - prend);
            s.erase(itc - 2, itc);
            prbegin -= 2;

            // move back another if we can, so we collapse any outer pair
            if( it != prbegin )
                --it;

            if (prbegin == prend)
                break;
        }

        danger = *it;
        danger ^= 0x20; // flip case yeah
    }

    return (int)(prbegin - prend);
}

int day5_2(const string& input)
{
    int min = 0x7fffff;
    for (char testc = 'a'; testc <= 'z'; ++testc)
    {
        string tests(input);
        tests.erase(remove_if(tests.begin(), tests.end(),
            [testc](char c) { return testc == (c | 0x20); }),
            tests.end());

        int collapsed = day5(tests);
        if (collapsed < min)
            min = collapsed;
    }
    
    return min;
}

// -------------------------------------------------------------------

struct D6Point
{
    int x, y;
    int area;
    int id;
    bool infinite;


    D6Point() { /**/ }
    D6Point(const string& line, int _id) : area(0), id(_id), infinite(false)
    {
        char comma;
        istringstream is(line);
        is >> x >> comma >> ws >> y;
    }

    int distfrom(int ox, int oy) const
    {
        return abs(ox - x) + abs(oy - y);
    }
};

int day6(const stringlist& input)
{
    // load input
    vector<D6Point> points;
    points.reserve(input.size());
    int id = 1;
    for (auto& line : input)
    {
        points.emplace_back(line, id);
        id++;
    }

    // scan points to find extents
    int minx = 1000000, miny = 1000000;
    int maxx = -1, maxy = -1;
    for (auto& point : points)
    {
        minx = min(minx, point.x);
        miny = min(miny, point.y);
        maxx = max(maxx, point.x);
        maxy = max(maxy, point.y);
    }

    // spin over the space and classify each point
    for (int y = miny; y <= maxy; ++y)
    {
        bool infinitey = (y == miny) || (y == maxy);

        for (int x = minx; x <= maxx; ++x)
        {
            D6Point* nearest = NULL;
            int mindist = 1000000000;

            for (auto& point : points)
            {
                int dist = point.distfrom(x, y);
                if (dist < mindist)
                {
                    mindist = dist;
                    nearest = &point;
                }
                else if (dist == mindist)
                {
                    nearest = NULL;
                }
            }

            if (nearest)
            {
                bool infinite = infinitey || (x == minx) || (x == maxx);

                nearest->infinite |= infinite;
                nearest->area++;
            }
        }
    }

    // now find the biggest non-infinite area
    const D6Point* nearest = NULL;
    int biggestarea = -1;
    for (auto point : points)
    {
        if (!point.infinite && point.area > biggestarea)
        {
            nearest = &point;
            biggestarea = point.area;
        }
    }

    return biggestarea;
}

int day6_2(const stringlist& input, int safedist)
{
    // load input
    vector<D6Point> points;
    points.reserve(input.size());
    int id = 1;
    for (auto& line : input)
    {
        points.emplace_back(line, id);
        id++;
    }

    // scan points to find extents
    int minx = 1000000, miny = 1000000;
    int maxx = -1, maxy = -1;
    for (auto& point : points)
    {
        minx = min(minx, point.x);
        miny = min(miny, point.y);
        maxx = max(maxx, point.x);
        maxy = max(maxy, point.y);
    }

    // whip over the whole space, then over all the points to measure safety
    // we going O(n^3) here o_O
    int safepoints = 0;
    for (int x = minx; x <= maxx; ++x)
    {
        for (int y = miny; y <= maxy; ++y)
        {
            int totaldist = 0;
            for (auto& point : points)
            {
                totaldist += point.distfrom(x, y);
                if (totaldist > safedist)
                    break;
            }
            if (totaldist < safedist) {
                safepoints++;
            }
        }
    }

    return safepoints;
}

// -------------------------------------------------------------------

int main()
{
    initcolours();
    srand((unsigned int)time(0));

    cout << GARLAND(2) << "  advent of code 2018  " << GARLAND(2) << endl;
    
    test(3, day1(READ("+1\n-2\n+3\n+1")));
    test(3, day1(READ("+1\n+1\n+1")));
    test(0, day1(READ("+1\n+1\n-2")));
    test(-6, day1(READ("-1\n-2\n-3")));
    gogogo(day1(LOAD(1)));

    test(0, day1_2(READ("+1\n-1")));
    test(10, day1_2(READ("+3\n+3\n+4\n-2\n-4")));
    test(5, day1_2(READ("-6\n+3\n+8\n+5\n-6")));
    test(14, day1_2(READ("+7\n+7\n-2\n-7\n-4")));
    gogogo(day1_2(LOAD(1)));

    test(12, day2(READ("abcdef\nbababc\nabbcde\nabcccd\naabcdd\nabcdee\nababab")));
    gogogo(day2(LOAD(2)));

    test<string>("fgij", day2_2(READ("abcde\nfghij\nklmno\npqrst\nfguij\naxcye\nwvxyz")));
    gogogo(day2_2(LOAD(2)));

    test(4, day3(READ("#1 @ 1,3: 4x4\n#2 @ 3, 1: 4x4\n#3 @ 5, 5: 2x2")));
    gogogo(day3(LOAD(3)));

    test(3, day3_2(READ("#1 @ 1,3: 4x4\n#2 @ 3, 1: 4x4\n#3 @ 5, 5: 2x2")));
    gogogo(day3_2(LOAD(3)));

    test(240, day4(LOAD(4t)));
    gogogo(day4(LOAD(4)));

    test(4455, day4_2(LOAD(4t)));
    gogogo(day4_2(LOAD(4)));
    
    test(0, day5(string("aA")));
    test(0, day5(string("abBA")));
    test(4, day5(string("abAB")));
    test(6, day5(string("aabAAB")));
    test(10, day5(string("dabAcCaCBAcCcaDA")));
    gogogo(day5(LOADSTR(5)));

    test(4, day5_2(string("dabAcCaCBAcCcaDA")));
    nononoD(day5_2(LOADSTR(5)));

    test(17, day6(READ("1, 1\n1, 6\n8, 3\n3, 4\n5, 5\n8, 9")));
    gogogo(day6(LOAD(6)));

    test(16, day6_2(READ("1, 1\n1, 6\n8, 3\n3, 4\n5, 5\n8, 9"), 32));
    gogogo(day6_2(LOAD(6), 10000));

    // animate snow falling behind the characters in the console until someone presses a key
    return twinkleforever();
}

