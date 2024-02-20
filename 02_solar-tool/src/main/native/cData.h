#ifndef CDATA_H_
#define CDATA_H_

#include <arg/core/cSerializableMatrix3.h>
#include <arg/core/cArray.h>

class cData
{

		// Sigma counts of target cols. for fitness evaluation
		// pre-computed at data-load time
		arg::cArrayConst<double> m_Sigmas;

		unsigned int m_Targets;
		unsigned int m_Inputs;

		unsigned int m_LeaveOutIdx;

		void UpdateSigmas(void);

	public:

		arg::matd3 m_Data;

		cData();
		cData(const unsigned x, const unsigned y);

		bool Load(const char * fname);

		inline unsigned int Records(void);
		inline unsigned int Inputs(void);
		inline unsigned int Targets(void);
		inline void TargetCount(const unsigned int val);

		inline double Sigma(const unsigned int idx) {return m_Sigmas[idx];};

		inline unsigned int LeaveOutIdx(void) {return m_LeaveOutIdx;};
		inline void LeaveOutIdx(const unsigned int val) {m_LeaveOutIdx = val;};

		inline double * Inputs(const unsigned int idx);
		inline double * Targets(const unsigned int idx);

		virtual ~cData();
};

inline void cData::TargetCount(const unsigned int val)
{
	if ( val!= m_Targets)
	{
		m_Targets = val;

		if (m_Data.Rows() > 0)
			UpdateSigmas();

		m_Inputs = m_Data.Cols() - m_Targets;
	}
}

inline unsigned int cData::Records(void)
{
	return m_Data.Rows();
}

inline unsigned int cData::Inputs(void)
{
	return m_Inputs = m_Data.Cols() - m_Targets;
}

inline unsigned int cData::Targets(void)
{
	return m_Targets;
}

inline double * cData::Inputs(const unsigned int i)
{
	return m_Data.Row(i);
}

inline double * cData::Targets(const unsigned int i)
{
	return m_Data.Row(i) + m_Inputs;
}

#endif /* CDATA_H_ */
