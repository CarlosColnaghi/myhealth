package com.myhealth.api.models;

import java.time.Instant;
import java.util.UUID;

import com.myhealth.api.enums.DeviceState;

import jakarta.persistence.Column;
import jakarta.persistence.Entity;
import jakarta.persistence.EnumType;
import jakarta.persistence.Enumerated;
import jakarta.persistence.GeneratedValue;
import jakarta.persistence.GenerationType;
import jakarta.persistence.Id;
import jakarta.persistence.Table;
import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;

@Entity
@Getter
@Setter
@NoArgsConstructor
@AllArgsConstructor
@Table(name = "readings")
public class Reading {
  @Id
  @GeneratedValue(strategy = GenerationType.UUID)
  private UUID id;
  private Instant time;
  @Enumerated(EnumType.STRING)
  private DeviceState state;
  private Double bpm;
  @Column(name = "device_id")
  private UUID deviceId;
}
