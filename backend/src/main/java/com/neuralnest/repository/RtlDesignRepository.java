package com.neuralnest.repository;

import com.neuralnest.model.RtlDesign;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

@Repository
public interface RtlDesignRepository extends JpaRepository<RtlDesign, Long> {
}
