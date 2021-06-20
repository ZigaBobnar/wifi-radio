import * as fs from "fs";
import { WaveFile } from "wavefile";
import { TrackDefinition } from "./TrackDefinition";
const MP3ToWav = require("mp3-to-wav");

class Track {
  isLoaded: boolean = false;
  isLoading: boolean = false;
  samples: Uint8Array = new Uint8Array();
  samplesCount: number = 0;
  chunkCount: number = 0;
  totalLengthMs: number = 0;

  constructor(
    public readonly definition: TrackDefinition,
    public readonly chunkSize: number,
    public readonly sampleRate: number
  ) {}

  async load(forceReload: boolean = false) {
    if ((this.isLoaded || this.isLoading) && !forceReload) return;

    this.isLoading = true;

    const path = this.definition.path;

    if (path.endsWith(".mp3")) {
      await this.loadMp3(path);
    } else {
      await this.loadWav(path);
    }

    this.isLoading = false;
  }

  async getInfo() {
    return {
      path: this.definition.path,
      title: this.definition.title,
      artist: this.definition.artist,
      isLoaded: this.isLoaded,
      samplesCount: this.samplesCount,
      sampleRate: this.sampleRate,
      chunkSize: this.chunkSize,
      chunkCount: this.chunkCount,
      totalLenghtMs: this.totalLengthMs,
    };
  }

  async getChunk(chunkIndex: number): Promise<Uint8Array> {
    if (!this.isLoaded) await this.load();

    return this.samples.slice(
      chunkIndex * this.chunkSize,
      (chunkIndex + 1) * this.chunkSize
    );
  }

  private async loadWav(path: string) {
    const wavFile = new WaveFile(fs.readFileSync(path));

    await this.processWaveFile(wavFile);
  }

  private async loadMp3(path: string) {
    const mp3Convert = new MP3ToWav(path);
    const decoded = await mp3Convert.decodeMp3(path);

    const wavFile = new WaveFile();
    wavFile.fromScratch(
      1 /* Mono */,
      // TODO: Actual sample rate does not work for some reason
      // Samplerate*2 works, but the track cuts off way before the end
      decoded.sampleRate * 2,
      "32",
      decoded.data[0].map((v: any) => v * 2147483647)
    );

    await this.processWaveFile(wavFile);
  }

  private async processWaveFile(file: WaveFile) {
    file.toBitDepth("8", true);
    file.toSampleRate(this.sampleRate);

    this.samples = new Uint8Array(file.getSamples(false, Int8Array));
    this.samplesCount = this.samples.length;
    this.chunkCount = Math.ceil(this.samplesCount / this.chunkSize);
    this.totalLengthMs = (1000 * this.samples.length) / this.sampleRate;
    this.isLoaded = true;
  }
}

export { Track };
