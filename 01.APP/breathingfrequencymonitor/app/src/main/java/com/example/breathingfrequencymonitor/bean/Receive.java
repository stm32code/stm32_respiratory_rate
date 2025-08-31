package com.example.breathingfrequencymonitor.bean;

public class Receive {
    private String breathe;
    private String waning;

    @Override
    public String toString() {
        return "Receive{" +
                "breathe='" + breathe + '\'' +
                ", waning='" + waning + '\'' +
                '}';
    }

    public String getBreathe() {
        return breathe;
    }

    public void setBreathe(String breathe) {
        this.breathe = breathe;
    }

    public String getWaning() {
        return waning;
    }

    public void setWaning(String waning) {
        this.waning = waning;
    }
}
