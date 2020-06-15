/*
 *  Copyright (c) hikyuu.org
 *
 *  Created on: 2020-6-15
 *      Author: fasiondog
 */

#pragma once

#include <hikyuu/indicator/build_in.h>
#include <pybind11/pybind11.h>
#include <hikyuu/data_driver/KDataDriver.h>
#include "../_Parameter.h"
#include "../pybind_utils.h"

using namespace hku;
namespace py = pybind11;

class PyKDataDriver : public KDataDriver {
public:
    using KDataDriver::KDataDriver;

    bool _init() {
        PYBIND11_OVERLOAD(bool, KDataDriver, _init, );
    }

    void loadKData(const string& market, const string& code, KQuery::KType kType, size_t start_ix,
                   size_t end_ix, KRecordListPtr out_buffer) {
        // PYBIND11_OVERLOAD(void, KDataDriver, loadKData, market, code, kType, start_ix, end_ix,
        //                  out_buffer);
        auto self = py::cast(this);
        py::list kr_list = self.attr("_loadKData")(market, code, kType, start_ix, end_ix);
        auto total = len(kr_list);
        for (auto i = 0; i < total; ++i) {
            out_buffer->push_back(kr_list[i].cast<KRecord>());
        }
    }

    size_t getCount(const string& market, const string& code, KQuery::KType kType) {
        PYBIND11_OVERLOAD(size_t, KDataDriver, getCount, market, code, kType);
    }

    bool getIndexRangeByDate(const string& market, const string& code, const KQuery& query,
                             size_t& out_start, size_t& out_end) {
        auto self = py::cast(this);
        py::tuple t = self.attr("_getIndexRangeByDate")(market, code, query);
        if (len(t) != 2) {
            PyErr_SetObject(PyExc_ValueError,
                            py::str("expected 2-item tuple in call to _getIndexRangeByDate; got {}")
                              .format(t)
                              .ptr());
            throw py::error_already_set();
        }

        long start_ix = 0, end_ix = 0;
        if (py::isinstance<py::int_>(t[0]) && py::isinstance<py::int_>(t[1])) {
            start_ix = t[0].cast<py::int_>();
            end_ix = t[0].cast<py::int_>();
        } else {
            throw std::logic_error("expected 2-int tuple in call to _getIndexRangeByDate");
        }

        if (start_ix < 0 && end_ix < 0) {
            throw std::logic_error("startix or endix must be >= zero!");
        }

        out_start = (size_t)start_ix;
        out_end = (size_t)end_ix;
        return true;
    }

    KRecord getKRecord(const string& market, const string& code, size_t pos, KQuery::KType kType) {
        PYBIND11_OVERLOAD(KRecord, KDataDriver, getKRecord, market, code, pos, kType);
    }

    TimeLineList getTimeLineList(const string& market, const string& code, const KQuery& query) {
        auto self = py::cast(this);
        py::list py_list = self.attr("_getTimeLineList")(market, code, query);
        return python_list_to_vector<TimeLineRecord>(py_list);
    }

    TransList getTransList(const string& market, const string& code, const KQuery& query) {
        auto self = py::cast(this);
        py::list py_list = self.attr("_getTransList")(market, code, query);
        return python_list_to_vector<TransRecord>(py_list);
    }
};