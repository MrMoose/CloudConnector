/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "ICloudTracing.h"


class BlindTrace final : public ICloudTrace {

	public:
		BlindTrace() = delete;
		BlindTrace(const BlindTrace &) = delete;
		~BlindTrace() noexcept;

		ScopedSegment scoped_segment(const FString &n_name) override;
		bool start_segment(const FString &n_name) override;
		bool end_segment(const FString &n_name, const bool n_error = false) noexcept override;
		const FString &id() const noexcept override;

	private:
		template <typename ObjectType, ESPMode Mode>
		friend class SharedPointerInternals::TIntrusiveReferenceController;
		friend class ICloudTracing;
		friend class BlindTracingImpl;

		BlindTrace(const FString &n_trace_id);

		const FString m_id;
};

