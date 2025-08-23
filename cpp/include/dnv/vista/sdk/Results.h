/**
 * @file Results.h
 * @brief Validation result types for Vista SDK operations
 * @details Provides discriminated union types for validation results
 *          following the same pattern as ShipId and DataChannelId
 */

#pragma once

namespace dnv::vista::sdk
{
	//=====================================================================
	// ValidateResult class
	//=====================================================================

	/** @brief Result of validation operation */
	class ValidateResult
	{
	public:
		class Ok final
		{
		public:
			Ok() = default;
		};

		class Invalid final
		{
		public:
			inline explicit Invalid( std::vector<std::string> errors ) noexcept;
			[[nodiscard]] inline const std::vector<std::string>& errors() const noexcept;

		private:
			std::vector<std::string> m_errors;
		};

		//-----------------------------
		// Construction from result
		//-----------------------------

		inline ValidateResult( Ok ok ) noexcept;
		inline ValidateResult( Invalid invalid ) noexcept;

		//-----------------------------
		// Type checking
		//-----------------------------

		[[nodiscard]] inline bool isOk() const noexcept;
		[[nodiscard]] inline bool isInvalid() const noexcept;

		//-----------------------------
		// Value access
		//-----------------------------

		[[nodiscard]] inline const Ok& ok() const;
		[[nodiscard]] inline const Invalid& invalid() const;

	private:
		std::variant<Ok, Invalid> m_value;
	};
}

#include "Results.inl"
