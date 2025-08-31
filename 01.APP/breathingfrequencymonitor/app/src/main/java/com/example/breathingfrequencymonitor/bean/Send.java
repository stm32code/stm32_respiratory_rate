package com.example.breathingfrequencymonitor.bean;

public class Send {
    private Integer cmd;
    private Integer open;

    @Override
    public String toString() {
        return "Send{" +
                "cmd=" + cmd +
                ", open=" + open +
                '}';
    }

    public Integer getCmd() {
        return cmd;
    }

    public void setCmd(Integer cmd) {
        this.cmd = cmd;
    }

    public Integer getOpen() {
        return open;
    }

    public void setOpen(Integer open) {
        this.open = open;
    }
}
