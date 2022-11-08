#pragma once

#include <functional>

#include "Types.h"
#include "ElementRanges.h"

template <typename ELEMENT_T, typename ELEMENT_DECIMAL_T>
class ElementRangeNext
{
public:
    explicit ElementRangeNext(ElementRanges<ELEMENT_T>& elementRanges
        , std::function<ELEMENT_DECIMAL_T& (ELEMENT_T&)> elementToDec)
        : elementRanges_(elementRanges)
        , elementToDec_(elementToDec)
    {
        lastElementsIt_ = elementRanges_.elements_.begin();
        lastRangesIt_ = elementRanges_.ranges_.begin();
    }

    void skipBefore(ELEMENT_T toElement)
    {
        ELEMENT_DECIMAL_T& toElementDec = elementToDec_(toElement);

        for (; lastElementsIt_ != elementRanges_.elements_.end(); ++lastElementsIt_) {
            ELEMENT_DECIMAL_T& element = elementToDec_(*lastElementsIt_);
            if (element >= toElementDec) {
                break;
            }
        }

        for (; lastRangesIt_ != elementRanges_.ranges_.end(); ++lastRangesIt_) {
            ELEMENT_DECIMAL_T& rangeFirst = elementToDec_(lastRangesIt_->first);
            ELEMENT_DECIMAL_T& rangeLast = elementToDec_(lastRangesIt_->second);

            if (toElementDec < rangeFirst) {
                break;
            }
            if (toElementDec >= rangeFirst && rangeLast >= toElementDec) {
                rangeFirst = toElementDec;
                break;
            }
        }
    }

    ELEMENT_T next()
    {
        static const ELEMENT_T ELEMENT_EMPTY = ELEMENT_T();
        if (isEof()) {
            return ELEMENT_EMPTY;
        }

        ELEMENT_DECIMAL_T* rangeStart = nullptr;
        ELEMENT_DECIMAL_T* rangeLast = nullptr;
        ELEMENT_DECIMAL_T* element = nullptr;

        if (lastRangesIt_ != elementRanges_.ranges_.end()) {
            rangeStart = &elementToDec_(lastRangesIt_->first);
            rangeLast = &elementToDec_(lastRangesIt_->second);
        }

        if (lastElementsIt_ != elementRanges_.elements_.end()) {
            element = &elementToDec_(*lastElementsIt_);
        }

        if (element && (!rangeStart || *element < *rangeStart)) {
            ELEMENT_T curElement = *lastElementsIt_;
            ++lastElementsIt_;
            return curElement;
        }

        if (element && (!rangeStart || *element == *rangeStart)) {
            ++lastElementsIt_;
        }

        if (*rangeStart < *rangeLast) {
            ELEMENT_T curElement = lastRangesIt_->first;
            (*rangeStart)++;
            return curElement;
        }
        else {
            ELEMENT_T curElement = lastRangesIt_->first;
            ++lastRangesIt_;
            return curElement;
        }

        return ELEMENT_EMPTY;
    }

    bool isEof() const
    {
        return lastRangesIt_ == elementRanges_.ranges_.end()
            && lastElementsIt_ == elementRanges_.elements_.end();
    }

private:
    ElementRanges<ELEMENT_T>& elementRanges_;
    std::function<ELEMENT_DECIMAL_T& (ELEMENT_T&)> elementToDec_;
    typename ElementRanges<ELEMENT_T>::ListElement::iterator lastElementsIt_;
    typename ElementRanges<ELEMENT_T>::ListRange::iterator lastRangesIt_;
};

