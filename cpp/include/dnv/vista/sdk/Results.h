/**
 * @file Results.h
 * @brief Validation result types for Vista SDK operations
 * @details Provides discriminated union types for validation results
 *          following the same pattern as ShipId and DataChannelId
 */

#pragma once

#include <string>
#include <variant>
#include <vector>

namespace dnv::vista::sdk
{
	//=====================================================================
	// ValidateResult class
	//=====================================================================

	/**
	 * @brief Discriminated union representing a validation result that can be either Ok or Invalid
	 */
	class ValidateResult
	{
	public:
		/**
		 * @brief Represents a successful validation result with no errors
		 */
		class Ok final
		{
		public:
			/**
			 * @brief Default constructor for successful validation result
			 */
			Ok() = default;
		};

		class Invalid final
		{
		public:
			/**
			 * @brief Constructs an invalid result with a list of validation errors
			 */
			inline explicit Invalid( std::vector<std::string> errors ) noexcept;

			/**
			 * @brief Gets the list of validation errors
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline const std::vector<std::string>& errors() const noexcept;

		private:
			std::vector<std::string> m_errors;
		};

		//-----------------------------
		// Construction from result
		//-----------------------------

		/**
		 * @brief Constructs a ValidateResult from a successful Ok result
		 */
		inline ValidateResult( Ok ok ) noexcept;

		/**
		 * @brief Constructs a ValidateResult from an Invalid result containing errors
		 */
		inline ValidateResult( Invalid invalid ) noexcept;

		//-----------------------------
		// Type checking
		//-----------------------------

		/**
		 * @brief Checks if this result represents a successful validation
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isOk() const noexcept;

		/**
		 * @brief Checks if this result represents a failed validation with errors
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isInvalid() const noexcept;

		//-----------------------------
		// Value access
		//-----------------------------

		/**
		 * @brief Gets the Ok result value, throws if this result is Invalid
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const Ok& ok() const;

		/**
		 * @brief Gets the Invalid result value, throws if this result is Ok
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const Invalid& invalid() const;

	private:
		std::variant<Ok, Invalid> m_value;
	};
}

#include "Results.inl"
