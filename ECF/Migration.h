#ifndef Migration_h
#define Migration_h

/**
 * \ingroup evol evoop
 * \brief	Migration class - handles individual migration between demes.
 *
 * Note: will be reimplemented to allow proprietary user-defined migration criteria.
 */
class Migration : public Operator
{
protected:
	StateP state_;
	uint migFrequency_;
	uint nEmigrants_;
	std::vector<SelectionOperatorP> selOp_;
	static const int BEST = 0;
	static const int RANDOM = 1;

public:
	/** 
	 * Performs migration with current parameters:
	 * - migration.freq:    migration occurs each 'migration.freq' generations
	 * - migration.number:  migrate this number of individuals from each deme
	 */
	bool operate(StateP);

	bool initialize(StateP);

	void registerParameters(StateP);

	virtual ~Migration()
	{	}
};
typedef boost::shared_ptr<Migration> MigrationP;

#endif	// Migration_h
