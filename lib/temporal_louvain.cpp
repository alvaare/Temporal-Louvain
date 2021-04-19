#include <iostream>
#include "temporal_louvain.hpp"

int temporal_partition::get_begin() {
    return begin;
}

int temporal_partition::get_end() {
    return end;
}

int temporal_partition::get_duration() {
    return end-begin;
}

void temporal_partition::set_begin(int date) {
    begin = date;
}

void temporal_partition::set_end(int date) {
    end = date;
}

vector<temporal_partition*>& history::get_content() {
    return content;
}

void history::insert_partition(temporal_partition* part) {
    content.push_back(part);
}