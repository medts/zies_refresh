#ifndef ERROR_STATUS_HPP_
#define ERROR_STATUS_HPP_

// =================================================================
class ErrorStatus
{
	public:
		ErrorStatus ()
		{
		}
		static void PowerUp ();
		static void UpdateBitMap ();
		static void PrintAll ();
};
extern ErrorStatus gErrorStatus;

#endif /* ERROR_STATUS_HPP_ */

