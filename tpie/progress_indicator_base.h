#ifndef _TPIE_PROGRESS_INDICATOR_BASE_H
#define _TPIE_PROGRESS_INDICATOR_BASE_H

#include <portability.h>
#include <algorithm>

namespace tpie {

///////////////////////////////////////////////////////////////////
///
///  \class progress_indicator_base
///
///  The base class for indicating the progress of some task.
///
///  \author The TPIE Project
///
///////////////////////////////////////////////////////////////////

    class progress_indicator_base {

    public:

	////////////////////////////////////////////////////////////////////
	///
	///  Initializes the indicator. There is a sanity check that 
	///  ensures that minRange <= maxRange and that stepValue 
	///  is in [1,maxRange-minRange].
	///
	///  \param  title        The title of the progress indicator.
	///  \param  description  A text to be printed in front of the 
	///                       indicator.
	///  \param  minRange     The lower bound of the counting range.
	///  \param  maxRange     The upper bound of the counting range.
	///  \param  stepValue    The increment for each step.
	///
	////////////////////////////////////////////////////////////////////

	progress_indicator_base(const std::string& title, 
							const std::string& description, 
							TPIE_OS_OFFSET minRange, 
							TPIE_OS_OFFSET maxRange, 
							TPIE_OS_OFFSET stepValue) : 
	    m_minRange(std::min(minRange, maxRange)),
	    m_maxRange(std::max(minRange, maxRange)),
	    m_stepValue(std::max(std::min(stepValue, (m_maxRange-m_minRange)), 
			    static_cast<TPIE_OS_OFFSET>(1))),
	    m_current(0),
	    m_percentageChecker(0), 
	    m_percentageValue(0), 
	    m_percentageUnit(0) {
	    // Do nothing.
	}

	progress_indicator_base(const progress_indicator_base& other) :
	    m_minRange(other.m_minRange),
	    m_maxRange(other.m_maxRange),
	    m_stepValue(other.m_stepValue),
	    m_current(other.m_current),
	    m_percentageChecker(other.m_percentageChecker),
	    m_percentageValue(other.m_percentageValue),
	    m_percentageUnit(other.m_percentageUnit)
	    {
		// Do nothing.
	    }

	progress_indicator_base& operator=(const progress_indicator_base& other) {
	    if (this != &other) {
		m_percentageChecker = other.m_percentageChecker;
		m_percentageValue   = other.m_percentageValue;
		m_percentageUnit    = other.m_percentageUnit;
		m_maxRange          = other.m_maxRange;
		m_minRange          = other.m_minRange;
		m_stepValue         = other.m_stepValue;
		m_current           = other.m_current;
	    }
	    return *this;
	}

	////////////////////////////////////////////////////////////////////
	///
	///  The destructor. Nothing is done.
	///
	////////////////////////////////////////////////////////////////////

	virtual ~progress_indicator_base() {
	    // Do nothing.
	};
    
	////////////////////////////////////////////////////////////////////
	///
	///  Simultaneously set the upper and lower bound of the counting
	///  range. Also, set the increment for each step. There is a sanity 
	///  check that ensures that minRange <= maxRange and that stepValue 
	///  is in [1,maxRange-minRange].
	///
	///  \param  minRange     The lower bound of the counting range.
	///  \param  maxRange     The upper bound of the counting range.
	///  \param  stepValue    The increment for each step.
	///
	////////////////////////////////////////////////////////////////////

	void set_range(TPIE_OS_OFFSET minRange, TPIE_OS_OFFSET maxRange, TPIE_OS_OFFSET stepValue) {
	    set_min_range(std::min(minRange, maxRange));
	    set_max_range(std::max(minRange, maxRange));
	    set_step_value( std::max(std::min(stepValue, (m_maxRange-m_minRange)),
				static_cast<TPIE_OS_OFFSET>(1)));
	    m_percentageValue = 0;
	    m_percentageChecker = 0;
	    m_percentageUnit = 0;
	    reset();
	}
    
	////////////////////////////////////////////////////////////////////
	///
	///  Simultaneously set the upper and lower bound of the counting
	///  range and set the increment to be max(1,0.01(maxRange-minRange)).
	///  There is a sanity check that ensures that minRange <= maxRange.
	///
	///  \param  minRange        The lower bound of the counting range.
	///  \param  maxRange        The upper bound of the counting range.
	///  \param  percentageUnit  1/percentageUnit is one "percent".
	///
	////////////////////////////////////////////////////////////////////

	void set_percentage_range(TPIE_OS_OFFSET minRange, TPIE_OS_OFFSET maxRange, unsigned short percentageUnit = 100) {
	    TPIE_OS_OFFSET localMin = std::min(minRange,maxRange);
	    TPIE_OS_OFFSET localMax = std::max(minRange,maxRange);
	    set_step_value(1);
	    m_percentageUnit  = std::max(percentageUnit, 
				    static_cast<unsigned short>(1));
	    m_percentageValue = (localMax-localMin)/m_percentageUnit;
	    if (m_percentageValue > 0) {
		set_min_range(0);
		set_max_range(m_percentageUnit);
	    }
	    else {
		set_min_range(localMin);
		set_max_range(localMax);
		m_percentageValue = 1;
		m_percentageUnit = static_cast<unsigned short>(localMax-localMin);
	    }
	    m_percentageChecker = 0;
	    reset();
	}

	////////////////////////////////////////////////////////////////////
	///
	///  Record an increment but only advance the indicator if it will
	///  be advance by at least one percent.
	///
	////////////////////////////////////////////////////////////////////

	void step_percentage() {
	    //  Increase the step counter.
	    ++m_percentageChecker;
	    m_percentageChecker = m_percentageChecker % m_percentageValue;

	    //  If the number of steps since the last update is large
	    //  enough to constiture one "percent", advance the indicator.
	    if ((!m_percentageChecker) && (m_current < m_maxRange)) 
		step();
	}

	////////////////////////////////////////////////////////////////////
	///
	///  Record an increment to the indicator and advance the indicator.
	///
	////////////////////////////////////////////////////////////////////

	void step() {
	    m_current += m_stepValue;
	    refresh();
	}

	////////////////////////////////////////////////////////////////////
	///
	///  Display a zero count. This method may also be used to 
	///  simultaneously set a new description.
	///
	////////////////////////////////////////////////////////////////////

	void init(const std::string& description = std::string()) {
	    m_current = m_minRange;
	    if (!description.empty()) {
			set_description(description);
	    }
	    refresh();
	}
    
	////////////////////////////////////////////////////////////////////
	///
	///  Reset the counter. The current position is reset to the
	///  lower bound of the counting range.
	///
	////////////////////////////////////////////////////////////////////

	virtual void reset() = 0;

	////////////////////////////////////////////////////////////////////
	///
	///  Advance the indicator to the end and print an (optional)
	///  message that is followed by a newline.
	///
	///  \param  text  The message to be printed at the end of the
	///                indicator.
	///
	////////////////////////////////////////////////////////////////////

	virtual void done(const std::string& text = std::string()) = 0;

	////////////////////////////////////////////////////////////////////
	///
	///  Set the lower bound of the counting range. This method
	///  also implies a reset of the counter. In order to be able
	///  to set the lower bound independent of setting the upper bound,
	///  no range checking is done.
	///
	///  \param  minRange  The new lower bound.
	///
	////////////////////////////////////////////////////////////////////

	virtual void set_min_range(TPIE_OS_OFFSET minRange) = 0;

	////////////////////////////////////////////////////////////////////
	///
	///  Set the upper bound of the counting range. This method
	///  also implies a reset of the counter. In order to be able
	///  to set the uper bound independent of setting the lower bound,
	///  no range checking is done.
	///
	///  \param  maxRange  The new upper bound.
	///
	////////////////////////////////////////////////////////////////////

	virtual void set_max_range(TPIE_OS_OFFSET maxRange) = 0;

	////////////////////////////////////////////////////////////////////
	///
	///  Set the increment by which the counter is advanced upon each
	///  call to step(). In order to be able to reset the counter,
	///  no range checking is done.
	///
	///  \param  stepValue  The incerement.
	///
	////////////////////////////////////////////////////////////////////

	virtual void set_step_value(TPIE_OS_OFFSET stepValue) = 0;
  
	////////////////////////////////////////////////////////////////////
	///
	///  Set the title of a new task to be monitored. The terminal
	///  line will be newline'd, and the title will be followed by a
	///  newline as well.
	///
	///  \param  title  The title of the new task to be monitored.
	///
	////////////////////////////////////////////////////////////////////

	virtual void set_title(const std::string& title) = 0;

	////////////////////////////////////////////////////////////////////
	///
	///  Set the description of the task currently being monitored.
	///  Invoking this method will clear the terminal line.
	///
	///  \param  description  The decription of the task being monitored.
	///
	////////////////////////////////////////////////////////////////////

	virtual void set_description(const std::string& description) = 0;

	////////////////////////////////////////////////////////////////////
	///
	///  Display the indicator.
	///
	////////////////////////////////////////////////////////////////////

	virtual void refresh() = 0;

    protected:

	/**  The lower bound of the counting range.  */
	TPIE_OS_OFFSET m_minRange;

	/**  The upper bound of the counting range.  */
	TPIE_OS_OFFSET m_maxRange;

	/**  The increment for each step.  */
	TPIE_OS_OFFSET m_stepValue;

	/**  The current progress count [m_minRange...m_maxRange].  */
	TPIE_OS_OFFSET m_current;

	/**  A temporary counter in [0...m_percentageValue-1].  */
	TPIE_OS_OFFSET m_percentageChecker;

	/**  The absolute value which constitutes one percent of 
	     the counting range.  */
	TPIE_OS_OFFSET m_percentageValue;

	/**  The unit in which "percentage" is measure. Default is
	     to measure in percent, i.e., the unit is 100. A value
	     other than 0 indicates that the counter is in percentage
	     mode, i.e., it displays percent instead of steps. */
	unsigned short m_percentageUnit;

    private:
	progress_indicator_base();
    };

}  //  tpie namespace

#endif // _TPIE_PROGRESS_INDICATOR_BASE
